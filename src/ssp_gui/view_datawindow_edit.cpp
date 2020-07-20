#include "view_datawindow_edit.h"
#include "ssp_gui.h"

const int ref1_sn=4;//sn值在表格列数
const int ref2_sn=2;
const int data1_sn=9;
const int data2_sn=9;
#define COL_REF1_SN      4
#define COL_REF2_DW_SN   2
#define COL_REF2_ROW_SN  3
#define COL_DATA1_SN     9
#define COL_DATA2_SN     9
view_datawindow_edit::view_datawindow_edit(QWidget *parent)
	: CBaseView(parent)
{
	ui.setupUi(this);
	m_inputDlg=new inputNum();
	m_countMax=1;
	pDb = DB;
	QStringList header;
	header<<tr("分类")<<tr("名称")<<tr("类型")<<tr("描述")<<tr("sn");
	ui.tableWidget_ref1->setColumnCount(5);
	ui.tableWidget_ref1->setHorizontalHeaderLabels(header);
	ui.tableWidget_ref1->setSelectionBehavior(QAbstractItemView::SelectRows);
	ui.tableWidget_ref1->setSelectionMode(QAbstractItemView::SingleSelection);
	ui.tableWidget_ref1->horizontalHeader()->setStretchLastSection(true);
	ui.tableWidget_ref1->verticalHeader()->hide();
	ui.tableWidget_ref1->setItemDelegate(new leftTableRefWindowDelegate(this));
	ui.tableWidget_ref1->setColumnWidth(0,100);
	ui.tableWidget_ref1->setColumnWidth(1,100);
	ui.tableWidget_ref1->setColumnWidth(2,100);
	ui.tableWidget_ref1->setColumnWidth(3,100);
	ui.tableWidget_ref1->setColumnWidth(4,100);
	ui.tableWidget_ref1->setColumnHidden(ref1_sn,true);//隐藏sn

	header.clear();
	header<<tr("序号")<<tr("名称")<<"sn" << "row_sn";
	ui.tableWidget_ref2->setColumnCount(4);
	ui.tableWidget_ref2->setHorizontalHeaderLabels(header);
	ui.tableWidget_ref2->setColumnHidden(ref2_sn,true);
	ui.tableWidget_ref2->setColumnHidden(COL_REF2_ROW_SN,true);
	ui.tableWidget_ref2->setColumnWidth(0,50);
	ui.tableWidget_ref2->setColumnWidth(1,100);
	ui.tableWidget_ref2->setColumnWidth(2,100);
	ui.tableWidget_ref2->setColumnWidth(3,100);
	ui.tableWidget_ref2->setSelectionBehavior(QAbstractItemView::SelectRows);
	ui.tableWidget_ref2->setSelectionMode(QAbstractItemView::SingleSelection);
	//ui.tableWidget_ref2->horizontalHeader()->setResizeMode(QHeaderView::ResizeToContents);
	ui.tableWidget_ref2->verticalHeader()->hide();
	

	header.clear();
	header<<tr("分类")<<tr("名称")<<tr("类型")<<tr("描述")<<tr("数据变换类型")<<tr("交叉数据集引用名")<<tr("交叉列序号")<<tr("交叉数据列序号")<<tr("扩展属性")<<"sn";
	ui.tableWidget_data1->setColumnCount(10);
	ui.tableWidget_data1->setHorizontalHeaderLabels(header);
	ui.tableWidget_data1->setSelectionBehavior(QAbstractItemView::SelectRows);
	ui.tableWidget_data1->setSelectionMode(QAbstractItemView::SingleSelection);
	ui.tableWidget_data1->horizontalHeader()->setStretchLastSection(true);
	ui.tableWidget_data1->verticalHeader()->hide();
	ui.tableWidget_data1->setItemDelegate(new leftTableDataWindowDelegate(this));
	ui.tableWidget_data1->setColumnWidth(0,100);
	ui.tableWidget_data1->setColumnWidth(1,50);
	ui.tableWidget_data1->setColumnWidth(2,100);
	ui.tableWidget_data1->setColumnWidth(3,100);
	ui.tableWidget_data1->setColumnWidth(4,100);
	ui.tableWidget_data1->setColumnWidth(5,100);
	ui.tableWidget_data1->setColumnWidth(6,100);
	ui.tableWidget_data1->setColumnWidth(7,100);
	ui.tableWidget_data1->setColumnWidth(8,100);
	ui.tableWidget_data1->setColumnWidth(9,150);
	ui.tableWidget_data1->setColumnHidden(data1_sn,true);//隐藏sn

	header.clear();
	header<<tr("序号")<<tr("名称")<<tr("类型")<<tr("主键")<<tr("宽度")<<tr("引用名称")<<tr("转换方法")<<tr("扩展属性")<<tr("序号BAK")<<"sn";
	ui.tableWidget_data2->setColumnCount(10);
	ui.tableWidget_data2->setHorizontalHeaderLabels(header);
	ui.tableWidget_data2->setColumnHidden(8,true);
	ui.tableWidget_data2->setColumnHidden(COL_DATA2_SN,true);
	ui.tableWidget_data2->setSelectionBehavior(QAbstractItemView::SelectRows);
	ui.tableWidget_data2->setSelectionMode(QAbstractItemView::SingleSelection);
	ui.tableWidget_data2->setColumnWidth(0,50);
	ui.tableWidget_data2->setColumnWidth(1,100);
	ui.tableWidget_data2->setColumnWidth(2,100);
	ui.tableWidget_data2->setColumnWidth(3,50);
	ui.tableWidget_data2->setColumnWidth(4,100);
	ui.tableWidget_data2->setColumnWidth(5,100);
	ui.tableWidget_data2->setColumnWidth(6,200);
	ui.tableWidget_data2->setColumnWidth(7,100);
	ui.tableWidget_data2->setColumnWidth(8,150);	
	ui.tableWidget_data2->setColumnWidth(9,100);	
	//ui.tableWidget_data2->horizontalHeader()->setResizeMode(QHeaderView::ResizeToContents);
	ui.tableWidget_data2->verticalHeader()->hide();
	ui.tableWidget_data2->setItemDelegate(new dataWindowDelegate(this));

	connect(ui.tableWidget_ref1,SIGNAL(currentItemChanged(QTableWidgetItem *,QTableWidgetItem *)),this,SLOT(showRefInfo(QTableWidgetItem *,QTableWidgetItem *)));
	connect(ui.tableWidget_ref1,SIGNAL(itemChanged(QTableWidgetItem *)),this,SLOT(editRef1(QTableWidgetItem *)));

	connect(ui.tableWidget_data1,SIGNAL(currentItemChanged(QTableWidgetItem *,QTableWidgetItem *)),this,SLOT(showDataInfo(QTableWidgetItem *,QTableWidgetItem *)));
	connect(ui.tableWidget_data1,SIGNAL(currentItemChanged(QTableWidgetItem *,QTableWidgetItem *)),this,SLOT(showConInfo(QTableWidgetItem *,QTableWidgetItem *)));
	connect(ui.tableWidget_data1,SIGNAL(itemChanged(QTableWidgetItem *)),this,SLOT(editData1(QTableWidgetItem *)));

	connect(ui.tableWidget_ref2,SIGNAL(currentItemChanged(QTableWidgetItem *,QTableWidgetItem *)),this,SLOT(ref2ItemCilcked(QTableWidgetItem *,QTableWidgetItem *)));
	connect(ui.tableWidget_ref2,SIGNAL(itemChanged(QTableWidgetItem *)),this,SLOT(editRef2(QTableWidgetItem *)));

	connect(ui.tableWidget_data2,SIGNAL(currentItemChanged(QTableWidgetItem *,QTableWidgetItem *)),this,SLOT(data2ItemCilcked(QTableWidgetItem *,QTableWidgetItem *)));
	connect(ui.tableWidget_data2,SIGNAL(itemChanged(QTableWidgetItem *)),this,SLOT(editData2(QTableWidgetItem *)));

	connect(ui.comboBox_data_cls,SIGNAL(currentIndexChanged(QString)),this,SLOT(dataCombo(QString)));
	connect(ui.comboBox_ref_cls,SIGNAL(currentIndexChanged(QString)),this,SLOT(refCombo(QString)));
	
	connect(ui.textEdit_ref,SIGNAL(textChanged()),this,SLOT(editRefText()));
	connect(ui.textEdit_data,SIGNAL(textChanged()),this,SLOT(editDataText()));

	connect(ui.pushButton_add,SIGNAL(clicked()),this,SLOT(onPushButton_add()));
	connect(ui.pushButton_copy,SIGNAL(clicked()),this,SLOT(onPushButton_copy()));
	connect(ui.pushButton_del,SIGNAL(clicked()),this,SLOT(onPushButton_del()));
	connect(ui.pushButton_view,SIGNAL(clicked()),this,SLOT(onPushButton_view()));
	connect(ui.pushButton_import,SIGNAL(clicked()),this,SLOT(onPushButton_import()));
	connect(ui.pushButton_export,SIGNAL(clicked()),this,SLOT(onPushButton_export()));
	connect(ui.pushButton_apply,SIGNAL(clicked()),this,SLOT(onPushButton_apply()));

	connect(ui.pushButton_ref2_add,SIGNAL(clicked()),this,SLOT(onPushButton_ref2_add()));
	connect(ui.pushButton_ref2_del,SIGNAL(clicked()),this,SLOT(onPushButton_ref2_del()));
	connect(ui.pushButton_data2_add,SIGNAL(clicked()),this,SLOT(onPushButton_data2_add()));
	connect(ui.pushButton_data2_del,SIGNAL(clicked()),this,SLOT(onPushButton_data2_del()));
	connect(ui.pushButton_newcol,SIGNAL(clicked()),this,SLOT(onPushButton_newcol()));

	connect(m_inputDlg,SIGNAL(finishInputNum()),this,SLOT(pasteCfg()));

	connect(ui.tabWidget,SIGNAL(currentChanged(int)),this,SLOT(tabChanged(int )));
	connect(ui.checkBox_useCls,SIGNAL(clicked()),this,SLOT(slotOnCheckBoxClick()));
	SQt::SetTableWidgetStyle(ui.tableWidget_ref1);
	SQt::SetTableWidgetStyle(ui.tableWidget_ref2);
	SQt::SetTableWidgetStyle(ui.tableWidget_data1);
	SQt::SetTableWidgetStyle(ui.tableWidget_data2);

	QList<int> widgetSizes1;//splitter间距调整
	widgetSizes1 <<450 << 550;
	ui.splitter->setSizes(widgetSizes1);
	ui.splitter_3->setSizes(widgetSizes1);
	QList<int> widgetSizes2;//splitter间距调整
	widgetSizes2 << 450 << 550;
	ui.splitter_2->setSizes(widgetSizes2);
	ui.splitter_4->setSizes(widgetSizes2);
	viewDwConEdit=new view_dw_condition_edit();
	QHBoxLayout *layout=new QHBoxLayout(this);
	layout->setContentsMargins(0,0,0,0);
	layout->addWidget(viewDwConEdit);
	ui.tab_5->setLayout(layout);
	refreshLeftTable();
	ui.tabWidget->setCurrentIndex(0);

}
void view_datawindow_edit::initRefCombo()
{
	ui.comboBox_ref_cls->blockSignals(true);
	SRecordset refcls;
	pDb->RetrieveRecordset("select distinct cls_name from t_ssp_data_window where dstype=1 and cls_name is not null order by cls_name desc,dw_desc",refcls);
	ui.comboBox_ref_cls->clear();
	ui.comboBox_ref_cls->addItem("全部");
	for(int i=0;i<refcls.GetRows();i++)
	{
		QString tmp=refcls.GetValue(i,0).data();
		if(tmp!="")
		{
			ui.comboBox_ref_cls->addItem(tmp);
		}
	}
	ui.comboBox_ref_cls->blockSignals(false);
}
void view_datawindow_edit::initDataCombo()
{
	ui.comboBox_data_cls->blockSignals(true);
	SRecordset datacls;
	pDb->RetrieveRecordset("select distinct cls_name from t_ssp_data_window where dstype=2 and cls_name is not null order by cls_name",datacls);
	
	ui.comboBox_data_cls->clear();
	ui.comboBox_data_cls->addItem("全部");
	for(int i=0;i<datacls.GetRows();i++)
	{
		QString tmp=datacls.GetValue(i,0).data();
		if(tmp!="")
		{
			ui.comboBox_data_cls->addItem(tmp);
		}
	}
	ui.comboBox_data_cls->blockSignals(false);
}
QTableWidget *view_datawindow_edit::getTableData2()
{
	return ui.tableWidget_data2;
}
QTableWidget *view_datawindow_edit::getTableData1()
{
	return ui.tableWidget_data1;
}
QTableWidget *view_datawindow_edit::getTableRef1()
{
	return ui.tableWidget_ref1;
}
void view_datawindow_edit::getComboxAndCheckState(QString& strComboText, bool& bIsChecked)
{
	strComboText = ui.comboBox_data_cls->currentText();
	bIsChecked = ui.checkBox_useCls->isChecked();
}
QString view_datawindow_edit::getCls()
{
	if(ui.checkBox_useCls->checkState()==Qt::Checked&&ui.comboBox_data_cls->currentText()!="全部")
	{
		QString cls=" and (cls_name is Null or cls_name='"+ui.comboBox_data_cls->currentText()+"')";
		return cls;
	}
	return "";
}
view_datawindow_edit::~view_datawindow_edit()
{
	delete m_inputDlg;
}
// ============================================================================
// function:view_datawindow_edit
// date:2015/08/24
// input:
// output:
// desc:根据在数据库内容刷新主表表格
// ============================================================================
void view_datawindow_edit::refreshLeftTable()
{
	setRefBlockSignals(true);
	setDataBlockSignals(true);
	ui.tableWidget_ref1->clearContents();
	ui.tableWidget_ref1->setRowCount(0);

	ui.tableWidget_data1->clearContents();
	ui.tableWidget_data1->setRowCount(0);

	m_rsLeftTable.clear();
	int ret = pDb->RetrieveRecordset("select dw_sn,dstype,name,dbtype,dw_desc,dw_select,transform_type,cross_refname,cross_column,cross_data_column,cls_name,ext_attr from t_ssp_data_window order by cls_name desc,dw_desc",m_rsLeftTable);
	if( ret < 0)
	{
		QMessageBox::warning(this,tr("警告"),tr("数据查询失败"),QMessageBox::Ok);
		return;
	} else if (ret == 0)
	{
		QMessageBox::warning(this,tr("警告"),tr("数据库无数据"),QMessageBox::Ok);
		return;
	}

	for(int i=0;i<m_rsLeftTable.GetRows();i++)
	{	
		if(m_rsLeftTable.GetValue(i,1)=="1")//类型为引用时
		{
			int currowcount = ui.tableWidget_ref1->rowCount();
			ui.tableWidget_ref1->insertRow(currowcount);
			QTableWidgetItem *item=new QTableWidgetItem(m_rsLeftTable.GetValue(i,10).data());
			ui.tableWidget_ref1->setItem(currowcount,0,item);

			item=new QTableWidgetItem(m_rsLeftTable.GetValue(i,4).data());
			ui.tableWidget_ref1->setItem(currowcount,3,item);

			item=new QTableWidgetItem(typeTransform(m_rsLeftTable.GetValue(i,3)).data());
			ui.tableWidget_ref1->setItem(currowcount,2,item);

			item=new QTableWidgetItem(m_rsLeftTable.GetValue(i,2).data());
			ui.tableWidget_ref1->setItem(currowcount,1,item);

 			item=new QTableWidgetItem(m_rsLeftTable.GetValue(i,0).data());
 			ui.tableWidget_ref1->setItem(currowcount,ref1_sn,item);
		}
		if(m_rsLeftTable.GetValue(i,1)=="2")//类型为数据集时
		{
			int currowcount = ui.tableWidget_data1->rowCount();
			ui.tableWidget_data1->insertRow(currowcount);
			QTableWidgetItem *item=new QTableWidgetItem(m_rsLeftTable.GetValue(i,10).data());
			ui.tableWidget_data1->setItem(currowcount,0,item);

			item=new QTableWidgetItem(m_rsLeftTable.GetValue(i,4).data());
			ui.tableWidget_data1->setItem(currowcount,3,item);

			item=new QTableWidgetItem(typeTransform(m_rsLeftTable.GetValue(i,3)).data());
			ui.tableWidget_data1->setItem(currowcount,2,item);

			item=new QTableWidgetItem(m_rsLeftTable.GetValue(i,2).data());
			ui.tableWidget_data1->setItem(currowcount,1,item);

			item=new QTableWidgetItem(m_rsLeftTable.GetValue(i,0).data());
			ui.tableWidget_data1->setItem(currowcount,data1_sn,item);
			//new
			item=new QTableWidgetItem(tranftranftype(m_rsLeftTable.GetValue(i,6)).data());
			ui.tableWidget_data1->setItem(currowcount,4,item);

			item=new QTableWidgetItem(m_rsLeftTable.GetValue(i,7).data());
			ui.tableWidget_data1->setItem(currowcount,5,item);

			item=new QTableWidgetItem(m_rsLeftTable.GetValue(i,8).data());
			ui.tableWidget_data1->setItem(currowcount,6,item);

			item=new QTableWidgetItem(m_rsLeftTable.GetValue(i,9).data());
			ui.tableWidget_data1->setItem(currowcount,7,item);
			item=new QTableWidgetItem(m_rsLeftTable.GetValue(i,11).data());
			ui.tableWidget_data1->setItem(currowcount,8,item);		}
	}
	setRefBlockSignals(false);
	setDataBlockSignals(false);

	initRefCombo();
	initDataCombo();
	ui.tableWidget_ref1->resizeColumnsToContents();
	ui.tableWidget_data1->resizeColumnsToContents();
	if (ui.tableWidget_ref1->rowCount() > 0)
	{
		ui.tableWidget_ref1->selectRow(0);
	}
	if (ui.tableWidget_data1->rowCount() > 0)
	{
		ui.tableWidget_data1->selectRow(0);
	}
	
}
//主表增加按钮响应
void view_datawindow_edit::onPushButton_add()
{
	QString clsName="";
	
	if(ui.tabWidget->currentIndex()==0)//引用
	{
		if(ui.comboBox_ref_cls->currentText()!="全部")
			clsName=ui.comboBox_ref_cls->currentText();
		ui.tableWidget_ref1->blockSignals(true);
		int curRow;
		curRow=ui.tableWidget_ref1->rowCount();
		if(ui.tableWidget_ref1->rowCount()==0)
			curRow=0;
		if(curRow==-1)
		{
			ui.tableWidget_ref1->blockSignals(false);
			return;
		}
		int curmaxdwsn = getMaxDwSn();
		if (curmaxdwsn < 0)
		{
			return;
		}
		curmaxdwsn++;

		bool flag=pDb->Execute(SString::toFormat("insert into t_ssp_data_window (dw_sn,dstype,name,dbtype,dw_desc,dw_select,cls_name) values (%d,%d,'',1,'','','%s')",curmaxdwsn,1,clsName.toStdString().data()));
		if(flag==false)
		{
			QMessageBox::warning(this,tr("警告"),tr("在数据库插入引用失败"),QMessageBox::Ok);
			ui.tableWidget_ref1->blockSignals(false);
			return;
		}
		ui.tableWidget_ref1->insertRow(curRow);
		QTableWidgetItem *item=new QTableWidgetItem(clsName);
		ui.tableWidget_ref1->setItem(curRow,0,item);
		item=new QTableWidgetItem();
		ui.tableWidget_ref1->setItem(curRow,1,item);
		item=new QTableWidgetItem(tr("固定"));
		ui.tableWidget_ref1->setItem(curRow,2,item);
		item=new QTableWidgetItem();
		ui.tableWidget_ref1->setItem(curRow,3,item);
		item=new QTableWidgetItem();
		item->setText(QString::number(curmaxdwsn));
		ui.tableWidget_ref1->setItem(curRow,ref1_sn,item);

		ui.tableWidget_ref1->blockSignals(false);
	}
	if(ui.tabWidget->currentIndex()==1)//数据集
	{
		if(ui.comboBox_data_cls->currentText()!="全部")
			clsName=ui.comboBox_data_cls->currentText();

		ui.tableWidget_data1->blockSignals(true);
		int curRow;
		curRow=ui.tableWidget_data1->rowCount();
		if(ui.tableWidget_data1->rowCount()==0)
			curRow=0;
		if(curRow==-1)
		{
			ui.tableWidget_data1->blockSignals(false);
			return;
		}
		int curmaxdwsn = getMaxDwSn();
		if (curmaxdwsn < 0)
		{
			return;
		}
		curmaxdwsn++;

		bool flag=pDb->Execute(SString::toFormat("insert into t_ssp_data_window (dw_sn,dstype,name,dbtype,dw_desc,dw_select,transform_type,cross_refname,cross_column,cross_data_column,cls_name) values (%d,%d,'',2,'','',0,'',0,0,'%s')",curmaxdwsn,2,clsName.toStdString().data()));
		if(flag==false)
		{
			QMessageBox::warning(this,tr("警告"),tr("在数据库插入数据集失败"),QMessageBox::Ok);
			ui.tableWidget_ref1->blockSignals(false);
			return;
		}
		ui.tableWidget_data1->insertRow(curRow);
		QTableWidgetItem *item=new QTableWidgetItem(clsName);
		ui.tableWidget_data1->setItem(curRow,0,item);
		item=new QTableWidgetItem();
		ui.tableWidget_data1->setItem(curRow,1,item);
		item=new QTableWidgetItem(tr("历史库"));
		ui.tableWidget_data1->setItem(curRow,2,item);
		item=new QTableWidgetItem();
		ui.tableWidget_data1->setItem(curRow,3,item);

		item=new QTableWidgetItem();
		item->setText(tr("不变换"));
		ui.tableWidget_data1->setItem(curRow,4,item);
		item=new QTableWidgetItem();
		ui.tableWidget_data1->setItem(curRow,5,item);
		item=new QTableWidgetItem();
		item->setText(QString::number(0));
		ui.tableWidget_data1->setItem(curRow,6,item);
		item=new QTableWidgetItem();
		item->setText(QString::number(0));
		ui.tableWidget_data1->setItem(curRow,7,item);

		item=new QTableWidgetItem();
		item->setText(QString::number(curmaxdwsn));
		ui.tableWidget_data1->setItem(curRow,data1_sn,item);

		ui.tableWidget_data1->blockSignals(false);
	}
}
void view_datawindow_edit::refCombo(QString text)
{
	setRefBlockSignals(true);
	ui.tableWidget_ref1->clearContents();
	ui.tableWidget_ref1->setRowCount(0);
	ui.tableWidget_ref2->clearContents();
	ui.tableWidget_ref2->setRowCount(0);
	ui.textEdit_ref->clear();
	m_rsLeftTable.clear();
	SString sql;
	if(text!="全部")
	{
		sql = SString::toFormat("select dw_sn,dstype,name,dbtype,dw_desc,dw_select,transform_type,cross_refname,\
								cross_column,cross_data_column,cls_name from t_ssp_data_window where cls_name='%s' and dstype=1 or (cls_name='' or cls_name is null) order by cls_name desc,dw_desc",text.toStdString().data());
	} else
	{
		sql = "select dw_sn,dstype,name,dbtype,dw_desc,dw_select,transform_type,cross_refname,\
								cross_column,cross_data_column,cls_name from t_ssp_data_window where dstype=1 order by cls_name desc,dw_desc";
	}
	int ret = pDb->RetrieveRecordset(sql, m_rsLeftTable);
	if(ret < 0)
	{
		QMessageBox::information(this,tr("警告"),tr("读取在数据库失败[%1]").arg(sql.data()),QMessageBox::Ok);
		setRefBlockSignals(false);
		return;
	} else if (ret == 0)
	{
		setRefBlockSignals(false);
		return;
	} 

	for(int i=0; i < m_rsLeftTable.GetRows();i++)
	{	
		ui.tableWidget_ref1->insertRow(i);
		QTableWidgetItem *item=new QTableWidgetItem(m_rsLeftTable.GetValue(i,10).data());
		ui.tableWidget_ref1->setItem(i,0,item);

		item=new QTableWidgetItem(m_rsLeftTable.GetValue(i,4).data());
		ui.tableWidget_ref1->setItem(i,3,item);

		item=new QTableWidgetItem(typeTransform(m_rsLeftTable.GetValue(i,3)).data());
		ui.tableWidget_ref1->setItem(i,2,item);

		item=new QTableWidgetItem(m_rsLeftTable.GetValue(i,2).data());
		ui.tableWidget_ref1->setItem(i,1,item);

		item=new QTableWidgetItem(m_rsLeftTable.GetValue(i,0).data());
		ui.tableWidget_ref1->setItem(i,ref1_sn,item);
	}
	setRefBlockSignals(false);
	ui.tableWidget_ref1->resizeColumnsToContents();
	if (ui.tableWidget_ref1->rowCount() > 0)
	{
		ui.tableWidget_ref1->selectRow(0);
	}
}
void view_datawindow_edit::dataCombo(QString text)
{
	setDataBlockSignals(true);
	ui.tableWidget_data1->clearContents();
	ui.tableWidget_data1->setRowCount(0);
	ui.tableWidget_data2->clearContents();
	ui.tableWidget_data2->setRowCount(0);
	ui.textEdit_data->clear();
	m_rsLeftTable.clear();
	SString sql;
	if(text!="全部")
	{
		sql = SString::toFormat("select dw_sn,dstype,name,dbtype,dw_desc,dw_select,transform_type,cross_refname,cross_column,cross_data_column,cls_name,ext_attr from t_ssp_data_window where cls_name='%s' and dstype=2 or (cls_name='' or cls_name is null) order by cls_name desc,dw_desc",text.toStdString().data());
	}
	else
	{
		sql = "select dw_sn,dstype,name,dbtype,dw_desc,dw_select,transform_type,cross_refname,cross_column,cross_data_column,cls_name,ext_attr from t_ssp_data_window where dstype=2 order by cls_name desc,dw_desc";
	}
	int ret = pDb->RetrieveRecordset(sql,m_rsLeftTable);
	if (ret < 0)
	{
		QMessageBox::information(this,tr("警告"),tr("读取在数据库失败[%1]").arg(sql.data()),QMessageBox::Ok);
		setDataBlockSignals(false);
		return;
	} else if (ret == 0)
	{
		return;
	} 
	for(int i=0;i<m_rsLeftTable.GetRows();i++)
	{	
		ui.tableWidget_data1->insertRow(i);
		QTableWidgetItem *item=new QTableWidgetItem(m_rsLeftTable.GetValue(i,10).data());
		ui.tableWidget_data1->setItem(i,0,item);

		item=new QTableWidgetItem(m_rsLeftTable.GetValue(i,4).data());
		ui.tableWidget_data1->setItem(i,3,item);

		item=new QTableWidgetItem(typeTransform(m_rsLeftTable.GetValue(i,3)).data());
		ui.tableWidget_data1->setItem(i,2,item);

		item=new QTableWidgetItem(m_rsLeftTable.GetValue(i,2).data());
		ui.tableWidget_data1->setItem(i,1,item);

		item=new QTableWidgetItem(m_rsLeftTable.GetValue(i,0).data());
		ui.tableWidget_data1->setItem(i,data1_sn,item);
		//new
		item=new QTableWidgetItem(tranftranftype(m_rsLeftTable.GetValue(i,6)).data());
		ui.tableWidget_data1->setItem(i,4,item);

		item=new QTableWidgetItem(m_rsLeftTable.GetValue(i,7).data());
		ui.tableWidget_data1->setItem(i,5,item);

		item=new QTableWidgetItem(m_rsLeftTable.GetValue(i,8).data());
		ui.tableWidget_data1->setItem(i,6,item);

		item=new QTableWidgetItem(m_rsLeftTable.GetValue(i,9).data());
		ui.tableWidget_data1->setItem(i,7,item);
		item=new QTableWidgetItem(m_rsLeftTable.GetValue(i,11).data());
		ui.tableWidget_data1->setItem(i,8,item);
	}
	setDataBlockSignals(false);
	ui.tableWidget_data1->resizeColumnsToContents();
	if (ui.tableWidget_data1->rowCount() > 0)
	{
		ui.tableWidget_data1->selectRow(0);
	}
}
//引用行添加按钮
void view_datawindow_edit::onPushButton_ref2_add()
{
	// 第一步获取主表当前选择行对象，获取dw_sn
	int ref1Row = ui.tableWidget_ref1->currentRow();
	if (ref1Row < 0)
	{
		QMessageBox::warning(this,tr("警告"),tr("未选择主表对象，请先选择主表对象"),QMessageBox::Ok);
		return;
	}
	QString strDwsn = ui.tableWidget_ref1->item(ref1Row,COL_REF1_SN)->text();
	int newRowsn = 0;
	SRecordset rst;
	SString sql;
	sql = SString::toFormat("select max(row_sn) from t_ssp_data_window_row where dw_sn=%s",strDwsn.toStdString().c_str());
	int iRet = pDb->RetrieveRecordset(sql,rst);
	if (iRet < 0)
	{
		QMessageBox::warning(this,tr("警告"),tr("在数据库执行失败[%1]").arg(sql.data()),QMessageBox::Ok);
		return;
	} else if (iRet == 0)
	{
		newRowsn = 1;
	} else {
		newRowsn = rst.GetValue(0,0).toInt() + 1;
	}
	//将新增数据插入数据库
	sql = SString::toFormat("insert into t_ssp_data_window_row (dw_sn,row_sn,row_id,row_name) values (%s,%d,'','')",strDwsn.toStdString().c_str(), newRowsn);

	bool flag=pDb->Execute(sql);
	if(flag==false)
	{
		QMessageBox::warning(this,tr("警告"),tr("在数据库ref插入row失败[%1]").arg(sql.data()),QMessageBox::Ok);
		return;
	}
	
	int curRefRow=ui.tableWidget_ref2->rowCount();
	ui.tableWidget_ref2->insertRow(curRefRow);
	ui.tableWidget_ref2->blockSignals(true);
	QTableWidgetItem *item=new QTableWidgetItem();
	ui.tableWidget_ref2->setItem(curRefRow,0,item);
	item=new QTableWidgetItem();
	ui.tableWidget_ref2->setItem(curRefRow,1,item);
	item=new QTableWidgetItem(strDwsn);
	ui.tableWidget_ref2->setItem(curRefRow,2,item);
	item=new QTableWidgetItem(QString::number(newRowsn));
	ui.tableWidget_ref2->setItem(curRefRow,3,item);
	ui.tableWidget_ref2->blockSignals(false);
}
//数据集列添加按钮
void view_datawindow_edit::onPushButton_data2_add()
{
	ui.tableWidget_data2->blockSignals(true);
	int curRow = ui.tableWidget_data2->rowCount();	

	int curdataRow=ui.tableWidget_data1->currentRow();
	if(curdataRow < 0)
	{	
		QMessageBox::warning(this,tr("警告"),tr("未选择主表，添加失败"),QMessageBox::Ok);
		ui.tableWidget_data2->blockSignals(false);
		return;
	}

	SRecordset tmpsets;
	int ret = pDb->RetrieveRecordset(SString::toFormat("select max(col_sn) from t_ssp_data_window_col where dw_sn=%d",ui.tableWidget_data1->item(curdataRow,data1_sn)->text().toInt()),tmpsets);
	int iNewNo = -1;
	if( ret < 0)
	{
		QMessageBox::warning(this,tr("警告"),tr("查询数据表数据失败"),QMessageBox::Ok);
		ui.tableWidget_data2->blockSignals(false);
		return;
	} else if (ret == 0)
	{
		iNewNo = 1;
	} else {
		iNewNo = tmpsets.GetValue(0,0).toInt()+1;
	}
	
	ui.tableWidget_data2->insertRow(curRow);
	QTableWidgetItem *item=new QTableWidgetItem();
	item->setText(ui.tableWidget_data1->item(curdataRow,data1_sn)->text());//主表选中行的sn
	ui.tableWidget_data2->setItem(curRow,COL_DATA2_SN,item);

	item=new QTableWidgetItem();
	item->setText(QString::number(iNewNo));
	//item->setFlags(item->flags()&~Qt::ItemIsEditable);
	ui.tableWidget_data2->setItem(curRow,0,item);
	item=new QTableWidgetItem();
	ui.tableWidget_data2->setItem(curRow,1,item);
	item=new QTableWidgetItem("字符型");
	ui.tableWidget_data2->setItem(curRow,2,item);
	item=new QTableWidgetItem();
	item->setCheckState(Qt::Unchecked);
	item->setFlags(item->flags()&~Qt::ItemIsEditable);
	ui.tableWidget_data2->setItem(curRow,3,item);
	item=new QTableWidgetItem();
	ui.tableWidget_data2->setItem(curRow,4,item);
	item=new QTableWidgetItem();
	ui.tableWidget_data2->setItem(curRow,5,item);
	item=new QTableWidgetItem();
	ui.tableWidget_data2->setItem(curRow,6,item);

	item=new QTableWidgetItem();
	ui.tableWidget_data2->setItem(curRow,7,item);
	item=new QTableWidgetItem();//BAK
	item->setText(QString::number(iNewNo));
	ui.tableWidget_data2->setItem(curRow,8,item);

	bool flag=pDb->Execute(SString::toFormat("insert into t_ssp_data_window_col (dw_sn,col_sn,col_name,col_vtype,col_width,ref_name,conv,pkey,isnull,ukey)	values (%s,%d,'','string',0,'','',0,0,0)",ui.tableWidget_data1->item(curdataRow,data1_sn)->text().toStdString().c_str(),iNewNo));
	if(flag==false)
		QMessageBox::warning(this,tr("警告"),tr("在数据库dataset插入col失败"),QMessageBox::Ok);

	for(int i=curRow+1;i<ui.tableWidget_data2->rowCount();i++)
	{
		int num=ui.tableWidget_data2->item(curRow,0)->text().toInt()+1;
		ui.tableWidget_data2->item(curRow,0)->setText(QString::number(num));
	}
	ui.tableWidget_data2->blockSignals(false);
}

//复制按钮
void view_datawindow_edit::onPushButton_copy()
{
	m_inputDlg->show();
}
//复制按钮响应
void view_datawindow_edit::pasteCfg()
{
	int num=m_inputDlg->getNum();//复制几次
	if(ui.tabWidget->currentIndex()==0)
	{
		int curRow=ui.tableWidget_ref1->currentRow();
		if(curRow>=0)
		{
			int newDwsn = getMaxDwSn();
			if (newDwsn < 0)
			{
				return;
			}		
			setRefBlockSignals(true);
			for(int i=0;i<num;i++)
			{
				newDwsn ++;
				QString strCurSelRowSn = ui.tableWidget_ref1->item(curRow,COL_REF1_SN)->text();
				//在数据库复制
				SString sql = SString::toFormat("insert into t_ssp_data_window (dw_sn,dstype,name,dbtype,dw_desc,dw_select,transform_type,cross_refname,cross_column,cross_data_column,cls_name,ext_attr) select %d,dstype,name,dbtype,dw_desc,dw_select,transform_type,cross_refname,cross_column,cross_data_column,cls_name,ext_attr from t_ssp_data_window where dw_sn=%s",
					newDwsn,strCurSelRowSn.toStdString().c_str());
				bool flag=pDb->Execute(sql);
				if(flag==false)
				{
					QMessageBox::warning(this,tr("警告"),tr("在数据库复制引用失败"),QMessageBox::Ok);
					setRefBlockSignals(false);
					return;
				}
				int icurRowCount = ui.tableWidget_ref1->rowCount();
				//表格复制
				ui.tableWidget_ref1->insertRow(icurRowCount);
				QTableWidgetItem *item=new QTableWidgetItem();
				QTableWidgetItem *pRef;
				pRef = ui.tableWidget_ref1->item(curRow,0);
				item->setText(pRef==NULL?"":pRef->text());
				ui.tableWidget_ref1->setItem(icurRowCount,0,item);

				item=new QTableWidgetItem();
				pRef = ui.tableWidget_ref1->item(curRow,1);
				item->setText(pRef==NULL?"":pRef->text());
				ui.tableWidget_ref1->setItem(icurRowCount,1,item);

				item=new QTableWidgetItem();
				pRef = ui.tableWidget_ref1->item(curRow,2);
				item->setText(pRef==NULL?"":pRef->text());
				ui.tableWidget_ref1->setItem(icurRowCount,2,item);

				item=new QTableWidgetItem();
				pRef = ui.tableWidget_ref1->item(curRow,3);
				item->setText(pRef==NULL?"":pRef->text());
				ui.tableWidget_ref1->setItem(icurRowCount,3,item);

				
				item=new QTableWidgetItem();
				item->setText(QString::number(newDwsn));
				ui.tableWidget_ref1->setItem(icurRowCount,COL_REF1_SN,item);
			
				//对应的row复制
				sql = SString::toFormat("insert into t_ssp_data_window_row (dw_sn,row_sn,row_id,row_name) select %d,row_sn,row_id,row_name from t_ssp_data_window_row where dw_sn=%s",
					newDwsn,strCurSelRowSn.toStdString().c_str());
				flag=pDb->Execute(sql);
				if(flag==false)
				{
					QMessageBox::warning(this,tr("警告"),tr("在数据库复制引用失败"),QMessageBox::Ok);
					setRefBlockSignals(false);
					return;
				}
			}
			setRefBlockSignals(false);
		}
	}
	if(ui.tabWidget->currentIndex()==1)//数据集
	{
		int curRow=ui.tableWidget_data1->currentRow();
		if(curRow>=0)
		{
			int newDwsn = getMaxDwSn();
			if (newDwsn < 0)
			{
				return;
			}
			setDataBlockSignals(true);
			for(int i=0;i<num;i++)
			{
				newDwsn++;
				QString strCurSelRowSn = ui.tableWidget_data1->item(curRow,COL_DATA1_SN)->text();
				//在数据库复制
				SString sql = SString::toFormat("insert into t_ssp_data_window (dw_sn,dstype,name,dbtype,dw_desc,dw_select,transform_type,cross_refname,cross_column,cls_name,cross_data_column,ext_attr) select %d,dstype,name,dbtype,dw_desc,dw_select,transform_type,cross_refname,cross_column,cls_name,cross_data_column,ext_attr from t_ssp_data_window where dw_sn=%s",
					newDwsn,strCurSelRowSn.toStdString().c_str());
				bool flag=pDb->Execute(sql);
				if(flag==false)
				{
					QMessageBox::warning(this,tr("警告"),tr("在数据库复制数据集失败"),QMessageBox::Ok);
					setDataBlockSignals(false);
					return;
				}
				int icurRowCount = ui.tableWidget_data1->rowCount();
				//表格复制
				ui.tableWidget_data1->insertRow(icurRowCount);
				QTableWidgetItem *item=new QTableWidgetItem();
				QTableWidgetItem *pRef;
				pRef = ui.tableWidget_data1->item(curRow,2);
				item->setText(pRef==NULL?"":pRef->text());
				ui.tableWidget_data1->setItem(icurRowCount,2,item);

				item=new QTableWidgetItem();
				pRef = ui.tableWidget_data1->item(curRow,1);
				item->setText(pRef==NULL?"":pRef->text());
				ui.tableWidget_data1->setItem(icurRowCount,1,item);

				item=new QTableWidgetItem();
				pRef = ui.tableWidget_data1->item(curRow,0);
				item->setText(pRef==NULL?"":pRef->text());
				ui.tableWidget_data1->setItem(icurRowCount,0,item);

				item=new QTableWidgetItem();
				pRef = ui.tableWidget_data1->item(curRow,3);
				item->setText(pRef==NULL?"":pRef->text());
				ui.tableWidget_data1->setItem(icurRowCount,3,item);

				item=new QTableWidgetItem();
				pRef = ui.tableWidget_data1->item(curRow,4);
				item->setText(pRef==NULL?"":pRef->text());
				ui.tableWidget_data1->setItem(icurRowCount,4,item);

				item=new QTableWidgetItem();
				pRef = ui.tableWidget_data1->item(curRow,5);
				item->setText(pRef==NULL?"":pRef->text());
				ui.tableWidget_data1->setItem(icurRowCount,5,item);

				item=new QTableWidgetItem();
				pRef = ui.tableWidget_data1->item(curRow,6);
				item->setText(pRef==NULL?"":pRef->text());
				ui.tableWidget_data1->setItem(icurRowCount,6,item);


				item=new QTableWidgetItem();
				pRef = ui.tableWidget_data1->item(curRow,7);
				item->setText(pRef==NULL?"":pRef->text());
				ui.tableWidget_data1->setItem(icurRowCount,7,item);

				item=new QTableWidgetItem();
				pRef = ui.tableWidget_data1->item(curRow,8);
				item->setText(pRef==NULL?"":pRef->text());
				ui.tableWidget_data1->setItem(icurRowCount,8,item);

				
				item=new QTableWidgetItem();
				item->setText(QString::number(newDwsn));
				ui.tableWidget_data1->setItem(icurRowCount,COL_DATA1_SN,item);
				
				//复制列定义
				sql = SString::toFormat("insert into t_ssp_data_window_col (dw_sn,col_sn,col_name,col_vtype,col_width,ref_name,conv,pkey,isnull,ukey,ext_attr) select %d,col_sn,col_name,col_vtype,col_width,ref_name,conv,pkey,isnull,ukey,ext_attr from t_ssp_data_window_col where dw_sn=%s",
					newDwsn,strCurSelRowSn.toStdString().c_str());
				flag=pDb->Execute(sql);
				if(flag==false)
				{
					QMessageBox::warning(this,tr("警告"),tr("在数据库复制列定义失败").arg(sql.data()),QMessageBox::Ok);
					setRefBlockSignals(false);
					return;
				}
				//复制条件定义
				sql = SString::toFormat("insert into t_ssp_data_window_condition (dw_sn,con_sn,con_name,cmp_type,input_type,is_option,ref_name,def_value) select %d,con_sn,con_name,cmp_type,input_type,is_option,ref_name,def_value from t_ssp_data_window_condition where dw_sn=%d",
					newDwsn,strCurSelRowSn.toStdString().c_str());
				flag=pDb->Execute(sql);
				if(flag==false)
				{
					QMessageBox::warning(this,tr("警告"),tr("在数据库复制条件定义引用失败").arg(sql.data()),QMessageBox::Ok);
					setRefBlockSignals(false);
					return;
				}
			}
			setDataBlockSignals(false);

		}
	}
}
//删除
void view_datawindow_edit::onPushButton_del()
{
	if(ui.tabWidget->currentIndex()==0)//引用
	{
		int curRow;
		if(ui.tableWidget_ref1->rowCount() == 0)
		{
			return;
		}
		curRow=ui.tableWidget_ref1->currentRow();
		if(curRow < 0)
		{
			QMessageBox::warning(this,tr("警告"),tr("主表中没有选中对象，请先选择对象！"),QMessageBox::Ok);
			return;
		}
		
		int i_ret = QMessageBox::warning(this,tr("警告"),tr("是否删除引用第")+QString::number(curRow+1)+tr("行？"),QMessageBox::Yes,QMessageBox::No);
		if (i_ret==QMessageBox::No)
		{
			return;
		}
		SString sn=ui.tableWidget_ref1->item(curRow,ref1_sn)->text().toStdString().data();
		bool flag=pDb->Execute(SString::toFormat("delete from t_ssp_data_window where dw_sn=%s",sn.data()));
		if(flag==false)
		{
			QMessageBox::warning(this,tr("警告"),tr("在数据库删除引用失败"),QMessageBox::Ok);
			return;
		}
		pDb->Execute(SString::toFormat("delete from t_ssp_data_window_row where dw_sn=%s",sn.data()));
		if(flag==false)
		{
			QMessageBox::warning(this,tr("警告"),tr("在数据库删除引用对应行失败"),QMessageBox::Ok);
			return;
		}
		ui.tableWidget_ref1->blockSignals(true);
		ui.tableWidget_ref1->removeRow(curRow);
		ui.tableWidget_ref1->blockSignals(false);
		QString strCurComText = ui.comboBox_ref_cls->currentText();
		if(strCurComText != "全部")
		{
			if (ui.tableWidget_ref1->rowCount() > 0)
			{
				ui.tableWidget_ref1->selectRow(0);

			} else {
				initRefCombo();
				refCombo("全部");
#if 0
				int iIndex = ui.comboBox_ref_cls->findText(strCurComText);
				if (iIndex < 0)
				{
					ui.comboBox_ref_cls->setCurrentIndex(0);
					
				} else {
					ui.comboBox_ref_cls->setCurrentIndex(iIndex);
				}
#endif
				
			}
		} else {
			if (ui.tableWidget_ref1->rowCount() > 0)
			{
				ui.tableWidget_ref1->selectRow(0);
			} else {
				initRefCombo();			
				ui.textEdit_ref->blockSignals(true);
				ui.textEdit_ref->clear();
				ui.textEdit_ref->blockSignals(false);
				ui.tableWidget_ref2->clearContents();
				ui.tableWidget_ref2->setRowCount(0);
			}
		}
	}
	if(ui.tabWidget->currentIndex()==1)//数据集
	{
		int curRow;
		if(ui.tableWidget_data1->rowCount() == 0)
		{
			return;
		}

		curRow=ui.tableWidget_data1->currentRow();
		if(curRow<0)
		{
			QMessageBox::warning(this,tr("警告"),tr("主表中没有选中对象，请先选择对象！"),QMessageBox::Ok);
			return;
		}
		int i_ret = QMessageBox::warning(this,tr("警告"),tr("是否删除数据集第")+QString::number(curRow+1)+tr("行？"),QMessageBox::Yes,QMessageBox::No);
		if (i_ret==QMessageBox::No)
		{
			return;
		}

		SString sn=ui.tableWidget_data1->item(curRow,data1_sn)->text().toStdString().data();
		bool flag=pDb->Execute(SString::toFormat("delete from t_ssp_data_window where dw_sn=%s",sn.data()));
		if(flag==false)
		{
			QMessageBox::warning(this,tr("警告"),tr("在数据库删除数据集失败"),QMessageBox::Ok);
			return;
		}
		flag = pDb->Execute(SString::toFormat("delete from t_ssp_data_window_col where dw_sn=%s",sn.data()));
		if(flag==false)
		{
			QMessageBox::warning(this,tr("警告"),tr("在数据库删除数据集失败"),QMessageBox::Ok);
			return;
		}

		flag = pDb->Execute(SString::toFormat("delete from t_ssp_data_window_condition where dw_sn=%s",sn.data()));
 		if(flag==false)
 		{
 			QMessageBox::warning(this,tr("警告"),tr("在数据库删除数据集条件失败"),QMessageBox::Ok);
 			return;
 		}
		ui.tableWidget_data1->blockSignals(true);
		ui.tableWidget_data1->removeRow(curRow);
		ui.tableWidget_data1->blockSignals(false);
		QString strCurComText = ui.comboBox_data_cls->currentText();
		if(strCurComText != "全部")
		{
			if (ui.tableWidget_data1->rowCount() > 0)
			{
				ui.tableWidget_data1->selectRow(0);

			} else {
				initDataCombo();
				dataCombo("全部");
				/*int iIndex = ui.comboBox_data_cls->findText(strCurComText);
				if (iIndex < 0)
				{
				ui.comboBox_data_cls->setEditText("全部");
				} else {
				ui.comboBox_data_cls->setCurrentIndex(iIndex);
				}*/
			}
		} else {
			if (ui.tableWidget_data1->rowCount() > 0)
			{
				ui.tableWidget_data1->selectRow(0);
			} else {
				initDataCombo();			
				ui.textEdit_data->blockSignals(true);
				ui.textEdit_data->clear();
				ui.textEdit_data->blockSignals(false);
				ui.tableWidget_data2->clearContents();
				ui.tableWidget_data2->setRowCount(0);
			}
		}
	}
}
//引用行删除
void view_datawindow_edit::onPushButton_ref2_del()
{
	int curRow = ui.tableWidget_ref2->currentRow();
	if(curRow < 0)
	{
		QMessageBox::warning(this,tr("警告"),tr("未选择主表"),QMessageBox::Ok);
		return;
	}
	int i_ret = QMessageBox::warning(this,tr("警告"),tr("是否删除引用对应行第")+QString::number(curRow+1)+tr("行？"),QMessageBox::Yes,QMessageBox::No);
	if (i_ret==QMessageBox::No)
	{
		return;
	}

	SString sn=ui.tableWidget_ref2->item(curRow,ref2_sn)->text().toStdString().data();
	QString strTemp = ui.tableWidget_ref2->item(curRow, COL_REF2_ROW_SN)->text();
	SString rowsn = ui.tableWidget_ref2->item(curRow, COL_REF2_ROW_SN)->text().toStdString().c_str();
	bool flag=pDb->Execute(SString::toFormat("delete from t_ssp_data_window_row where dw_sn=%s and row_sn=%s",sn.data(), rowsn.data()));
	if(flag==false)
	{
		QMessageBox::warning(this,tr("警告"),tr("在数据库删除引用对应行失败"),QMessageBox::Ok);
		ui.tableWidget_ref2->blockSignals(false);
		return;
	}
	ui.tableWidget_ref2->blockSignals(true);
	ui.tableWidget_ref2->removeRow(curRow);
	ui.tableWidget_ref2->blockSignals(false);
	if (ui.tableWidget_ref2->rowCount() <= 0)
	{
		ui.pushButton_ref2_del->setEnabled(false);
	}

}
//数据集列删除
void view_datawindow_edit::onPushButton_data2_del()
{
	int curRow;
	ui.tableWidget_data2->blockSignals(true);
	curRow=ui.tableWidget_data2->currentRow();
	if(curRow<0)
	{
		ui.tableWidget_data2->blockSignals(false);
		return;
	}
	int i_ret = QMessageBox::warning(this,tr("警告"),tr("是否删除数据集列第")+QString::number(curRow+1)+tr("行？"),QMessageBox::Yes,QMessageBox::No);
	if (i_ret==QMessageBox::No)
	{
		ui.tableWidget_data2->blockSignals(false);
		return;
	}
	//int curdataRow=ui.tableWidget_data2->currentRow();
	SString sn=ui.tableWidget_data2->item(curRow,COL_DATA2_SN)->text().toStdString().data();
	SString ssql=SString::toFormat("delete from t_ssp_data_window_col where dw_sn=%s",sn.data());
	bool flag=pDb->Execute(SString::toFormat("delete from t_ssp_data_window_col where dw_sn=%s and col_sn=%s",sn.data(),ui.tableWidget_data2->item(curRow,0)->text().toStdString().data()));
	if(flag==false)
	{
		QMessageBox::warning(this,tr("警告"),tr("在数据库删除数据集自有列失败"),QMessageBox::Ok);
		ui.tableWidget_data2->blockSignals(false);
		return;
	}

	ui.tableWidget_data2->removeRow(curRow);
	ui.tableWidget_data2->blockSignals(false);
	if (ui.tableWidget_ref2->rowCount() <= 0)
	{
		ui.pushButton_data2_del->setEnabled(false);
	}
}
void view_datawindow_edit::onPushButton_newcol()
{
	int curdataRow=ui.tableWidget_data1->currentRow();
	if(curdataRow<0)
	{	
		QMessageBox::warning(this,tr("警告"),tr("未选择主表，添加失败"),QMessageBox::Ok);
		return;
	}
	QString strDwsn = ui.tableWidget_data1->item(curdataRow,COL_DATA1_SN)->text();
	ui.tableWidget_data2->blockSignals(true);
	if(ui.tableWidget_data2->rowCount()>0)
	{
		if(QMessageBox::No==QMessageBox::warning(this,tr("警告"),tr("已存在列,是否清空？"),QMessageBox::Yes,QMessageBox::No))
			return;
		SString ssql=SString::toFormat("delete from t_ssp_data_window_col where dw_sn=%s",strDwsn.toStdString().c_str());
		bool bFlag = pDb->Execute(ssql);
		if (!bFlag)
		{
			QMessageBox::warning(this,tr("警告"),tr("删除数据库数据失败[%1]").arg(ssql.data()),QMessageBox::Ok);
			ui.tableWidget_data2->blockSignals(false);
			return;
		}
		ui.pushButton_data2_del->setEnabled(false);
		ui.tableWidget_data2->clearContents();
		ui.tableWidget_data2->setRowCount(0);
	}

	QString str=ui.textEdit_data->toPlainText();
	if(!str.contains("select"))
	{
		ui.tableWidget_data2->blockSignals(false);
		return;
	}
	QStringList strList=str.split("from").at(0).split("select").at(1).split(",");
	int num=strList.size();	
	
	for(int i = 0;i < num; i++)
	{
		int no = i + 1;
		//先做数据库插入，插入成功了在表中显示
		bool flag=pDb->Execute(SString::toFormat("insert into t_ssp_data_window_col (dw_sn,col_sn,col_name,col_vtype,col_width,ref_name,conv,pkey,isnull,ukey)	values (%s,%d,'%s','string',0,'','',0,0,0)",strDwsn.toStdString().c_str(),no,strList.at(i).toStdString().c_str()));
		if(flag==false)
		{
			QMessageBox::warning(this,tr("警告"),tr("在数据库dataset插入col失败"),QMessageBox::Ok);
			ui.tableWidget_data2->blockSignals(false);
			return;
		}
		ui.tableWidget_data2->insertRow(i);
		QTableWidgetItem *item=new QTableWidgetItem();
		item->setText(QString::number(no));
		ui.tableWidget_data2->setItem(i,0,item);
		item=new QTableWidgetItem(strList.at(i));
		ui.tableWidget_data2->setItem(i,1,item);
		item=new QTableWidgetItem("字符型");
		ui.tableWidget_data2->setItem(i,2,item);
		item=new QTableWidgetItem();
		item->setCheckState(Qt::Unchecked);
		item->setFlags(item->flags()&~Qt::ItemIsEditable);
		ui.tableWidget_data2->setItem(i,3,item);
		item=new QTableWidgetItem();
		ui.tableWidget_data2->setItem(i,4,item);
		item=new QTableWidgetItem();
		ui.tableWidget_data2->setItem(i,5,item);
		item=new QTableWidgetItem();
		ui.tableWidget_data2->setItem(i,6,item);
		item=new QTableWidgetItem();
		ui.tableWidget_data2->setItem(i,7,item);

		item=new QTableWidgetItem();//BAK
		item->setText(QString::number(no));
		ui.tableWidget_data2->setItem(i,8,item);

		item=new QTableWidgetItem();
		item->setText(strDwsn);//主表选中行的sn
		ui.tableWidget_data2->setItem(i,COL_DATA2_SN,item);	
	}

	ui.tableWidget_data2->blockSignals(false);
	if (ui.tableWidget_data2->rowCount() > 0)
	{
		ui.tableWidget_data2->selectRow(0);
		ui.pushButton_data2_del->setEnabled(true);
	}
}
//预览
void view_datawindow_edit::onPushButton_view()
{
	
	if(ui.tabWidget->currentIndex()==0)
	{
		QMessageBox::warning(this,tr("警告"),tr("引用不可预览"),QMessageBox::Yes);
		return;
	}
	else if(ui.tabWidget->currentIndex()==1)
	{
		
		int curRow=ui.tableWidget_data1->currentRow();
		if(curRow<0)
		{
			QMessageBox::warning(this,tr("警告"),tr("未选择主表"),QMessageBox::Ok);
			return;
		}
		if(ui.tableWidget_data2->rowCount()<1)
		{
			QMessageBox::warning(this,tr("警告"),tr("列为空"),QMessageBox::Ok);
			return;
		}

		review reviewDlg(this);	
		view_common *common=new view_common(reviewDlg.widget,ui.tableWidget_data1->item(curRow,1)->text().toStdString().data());
		common->InitWidget(ui.tableWidget_data1->item(curRow,1)->text().toStdString().data());
		view_query *query=new view_query(reviewDlg.widget_2,ui.tableWidget_data1->item(curRow,1)->text().toStdString().data(),false);
		query->InitWidget(ui.tableWidget_data1->item(curRow,1)->text().toStdString().data());
 		reviewDlg.gridLayout_2->addWidget(common);
 		reviewDlg.gridLayout_4->addWidget(query);
		reviewDlg.exec();
	}
	
}
int view_datawindow_edit::getMaxDwSn()
{
	int maxDwsn = -1;
	SString sql = "select max(dw_sn) from t_ssp_data_window";
	SRecordset tempSet;
	int ret = pDb->RetrieveRecordset(sql,tempSet);
	if (ret < 0)
	{
		QMessageBox::warning(this,tr("警告"),tr("数据库查询失败【%1】").arg(sql.data()),QMessageBox::Ok);
		return -1;
	} else if (ret == 0)
	{
		maxDwsn = 0;
	}else{
		maxDwsn = tempSet.GetValue(0,0).toInt();
	}
	return maxDwsn;
}
bool view_datawindow_edit::insertOneRefSetInfo(int maxDwsn, myRef* curRefSet)
{
	SString sql;
	if (curRefSet->dbtype == "1")
	{
		sql = SString::toFormat("insert into t_ssp_data_window \
								(dw_sn,dstype,name,dbtype,dw_desc,dw_select,transform_type,cross_refname,\
								cross_column,cross_data_column,cls_name,ext_attr) values (%d,%d,'%s',%s,'%s',' ',0,' ',0,0,'%s',' ')"
								,maxDwsn, 1, curRefSet->name.data(),curRefSet->dbtype.data(),curRefSet->dw_desc.data(),curRefSet->cls_name.data());
		if (!pDb->Execute(sql))
		{
			QMessageBox::warning(this,tr("警告"),tr("在数据库中插入失败[%1]").arg(sql.data()),QMessageBox::Ok);
			return false;
		}
		for (int i = 0; i < curRefSet->myRowList.count(); i ++)
		{
			sql = SString::toFormat("insert into t_ssp_data_window_row (dw_sn,row_sn,row_id,row_name) values \
									(%d,%d,'%s','%s')",maxDwsn, i+1,curRefSet->myRowList.at(i)->id.data(),curRefSet->myRowList.at(i)->name.data());
			if (!pDb->Execute(sql))
			{
				QMessageBox::warning(this,tr("警告"),tr("在数据库中插入失败[%1]").arg(sql.data()),QMessageBox::Ok);
				return false;
			}
		}
	} else {
		sql = SString::toFormat("insert into t_ssp_data_window \
								(dw_sn,dstype,name,dbtype,dw_desc,dw_select,transform_type,cross_refname,\
								cross_column,cross_data_column,cls_name,ext_attr) values (%d,%d,'%s',%s,'%s','%s',0,' ',0,0,'%s',' ')"\
								,maxDwsn, 1, curRefSet->name.data(),curRefSet->dbtype.data(),curRefSet->dw_desc.data(),curRefSet->dw_select.data(),curRefSet->cls_name.data());
		if (!pDb->Execute(sql))
		{
			QMessageBox::warning(this,tr("警告"),tr("在数据库中插入失败[%1]").arg(sql.data()),QMessageBox::Ok);
			return false;
		}
	}
	return true;
}
bool view_datawindow_edit::handleOneRefNode(int& maxDwsn, myRef* curRefSet)
{
	SString sql = SString::toFormat("select dw_sn from t_ssp_data_window where name='%s' and dstype=1",curRefSet->name.data());
	SRecordset querySet;
	int ret = pDb->RetrieveRecordset(sql,querySet);
	if (ret < 0)
	{
		QMessageBox::warning(this,tr("警告"),tr("在数据库查询失败[%1]").arg(sql.data()),QMessageBox::Ok);
		return false;
	} else if (ret == 0)
	{
		maxDwsn ++;
		return insertOneRefSetInfo(maxDwsn,curRefSet);
	} else 
	{
		//这里防止原数据库存在name重复或是导入的数据存在重复，起到清除所有原数据，防止导入后存在重复，所有做多次判断
		for (int i = 0; i < querySet.GetRows(); i++)
		{
			sql = SString::toFormat("delete from t_ssp_data_window where dw_sn=%s",querySet.GetValue(i,0).data());
			if (!pDb->Execute(sql))
			{
				QMessageBox::warning(this,tr("警告"),tr("在数据库中删除失败[%1]").arg(sql.data()),QMessageBox::Ok);
				return false;
			}
			if (curRefSet->dbtype == "1")
			{
				sql = SString::toFormat("delete from t_ssp_data_window_row where dw_sn=%s",querySet.GetValue(i,0).data());
				if (!pDb->Execute(sql))
				{
					QMessageBox::warning(this,tr("警告"),tr("在数据库中删除失败[%1]").arg(sql.data()),QMessageBox::Ok);
					return false;
				}
			}
		}
		maxDwsn ++;
		return insertOneRefSetInfo(maxDwsn,curRefSet);
	}
	return true;
}
bool view_datawindow_edit::insertOneDataSetInfo(int maxDwsn, myDataset* curDataSet)
{
	SString sql = SString::toFormat("insert into t_ssp_data_window \
							(dw_sn,dstype,name,dbtype,dw_desc,dw_select,transform_type,cross_refname,\
							cross_column,cross_data_column,cls_name,ext_attr) values (%d,%d,'%s',%d,'%s','%s',%d,'%s',%d,%d,'%s','%s')"\
							,maxDwsn, 2, curDataSet->name.data(),curDataSet->dbtype,curDataSet->dw_desc.data(),curDataSet->dw_select.data()
							,curDataSet->transform_type,curDataSet->cross_refname.data(),curDataSet->cross_column,curDataSet->cross_data_column
							,curDataSet->cls_name.data(),curDataSet->ext_attr.data());
	if (!pDb->Execute(sql))
	{
		QMessageBox::warning(this,tr("警告"),tr("在数据库中插入失败[%1]").arg(sql.data()),QMessageBox::Ok);
		return false;
	}
	for (int i = 0; i < curDataSet->myColList.count(); i ++)
	{
		sql = SString::toFormat("insert into t_ssp_data_window_col (dw_sn,col_sn,col_name,col_vtype,col_width,ref_name,conv,pkey,isnull,ukey,ext_attr) values \
								(%d,%d,'%s','%s',%d,'%s','%s',%d,%d,%d,'%s')",
								maxDwsn, i+1,curDataSet->myColList.at(i)->col_name.data(), curDataSet->myColList.at(i)->col_vtype.data(),
								curDataSet->myColList.at(i)->col_width,curDataSet->myColList.at(i)->ref_name.data(),curDataSet->myColList.at(i)->conv.data(),
								curDataSet->myColList.at(i)->pkey,curDataSet->myColList.at(i)->isnull,curDataSet->myColList.at(i)->ukey,
								curDataSet->myColList.at(i)->ext_attr.data());
		if (!pDb->Execute(sql))
		{
			QMessageBox::warning(this,tr("警告"),tr("在数据库中插入失败[%1]").arg(sql.data()),QMessageBox::Ok);
			return false;
		}
	}

	for (int i = 0; i < curDataSet->myConditionList.count(); i++)
	{
		sql = SString::toFormat("insert into t_ssp_data_window_condition (dw_sn,con_sn,con_name,is_option,cmp_type,input_type,ref_name,def_value,dyn_where) values \
								(%d,%d,'%s',%d,%d,%d,'%s','%s','%s')",maxDwsn, i+1, curDataSet->myConditionList.at(i)->con_name.data(),
								curDataSet->myConditionList.at(i)->is_option, curDataSet->myConditionList.at(i)->cmp_type, curDataSet->myConditionList.at(i)->input_type,
								curDataSet->myConditionList.at(i)->ref_name.data(), curDataSet->myConditionList.at(i)->def_value.data(), curDataSet->myConditionList.at(i)->dyn_where.data());
		if (!pDb->Execute(sql))
		{
			QMessageBox::warning(this,tr("警告"),tr("在数据库中插入失败[%1]").arg(sql.data()),QMessageBox::Ok);
			return false;
		}
	}
	return true;
	
}
bool view_datawindow_edit::handleOneDataNode(int& maxDwsn, myDataset* curDataSet)
{
	SString sql = SString::toFormat("select dw_sn from t_ssp_data_window where name='%s' and dstype=2",curDataSet->name.data());
	SRecordset querySet;
	int ret = pDb->RetrieveRecordset(sql,querySet);
	if (ret < 0)
	{
		QMessageBox::warning(this,tr("警告"),tr("在数据库查询失败[%1]").arg(sql.data()),QMessageBox::Ok);
		return false;
	} else if (ret == 0)
	{
		maxDwsn ++;
		return insertOneDataSetInfo(maxDwsn,curDataSet);
	} else 
	{
		//这里防止原数据库存在name重复或是导入的数据存在重复，起到清除所有原数据，防止导入后存在重复，所有做多次判断
		for (int i = 0; i < querySet.GetRows(); i++)
		{
			sql = SString::toFormat("delete from t_ssp_data_window where dw_sn=%s",querySet.GetValue(i,0).data());
			if (!pDb->Execute(sql))
			{
				QMessageBox::warning(this,tr("警告"),tr("在数据库中删除失败[%1]").arg(sql.data()),QMessageBox::Ok);
				return false;
			}

	
			sql = SString::toFormat("delete from t_ssp_data_window_col where dw_sn=%s",querySet.GetValue(i,0).data());
			if (!pDb->Execute(sql))
			{
				QMessageBox::warning(this,tr("警告"),tr("在数据库中删除失败[%1]").arg(sql.data()),QMessageBox::Ok);
				return false;
			}

			sql = SString::toFormat("delete from t_ssp_data_window_condition where dw_sn=%s",querySet.GetValue(i,0).data());
			if (!pDb->Execute(sql))
			{
				QMessageBox::warning(this,tr("警告"),tr("在数据库中删除失败[%1]").arg(sql.data()),QMessageBox::Ok);
				return false;
			}
		}
		maxDwsn ++;
		return insertOneDataSetInfo(maxDwsn,curDataSet);
	}
	return true;
}
//导入
void view_datawindow_edit::onPushButton_import()
{
	QString path=QFileDialog::getOpenFileName(this,"打开文件","./../conf/sys_datawindow.xml","*.xml",0,0);
	//QString path="D:\\softPlat\\trunk\\SSP01\\project\\win32-vc10\\sys_datawindow.xml";
	SXmlConfig xml;
	if(!xml.ReadConfig( path.toStdString().data()))
	{
		LOGERROR("Load Datawindow config error!file=%s",path.toStdString().data());
		return;
	}
	int curMaxdwsn = getMaxDwSn();
	if (curMaxdwsn < 0)
	{
		return;
	}

	SBaseConfig *pDW=&xml;

	if(pDW->GetNodeName() != "data_window")
	{
		LOGERROR("Datawindow root node is not 'data_window'!");
		return;
	}
	SSP_GUI->ShowWaiting();
#if 1
	int nodeCount = pDW->GetChildCount();
	for (int i = 0; i < nodeCount; i++)
	{
		SBaseConfig *psubNode = pDW->GetChildNode(i);
		if (psubNode == NULL)
		{
			continue;
		}
		if (psubNode->GetNodeName() == "reference")
		{
			myRef ref;
			ref.name = psubNode->GetAttribute("name");
			SString strDbType=psubNode->GetAttribute("dbtype");
			if(strDbType == "fixed")
				ref.dbtype = "1";
			else if(strDbType == "db")
				ref.dbtype = "2";
			else if(strDbType == "mdb")
				ref.dbtype = "3";
			else
			{
				LOGWARN("Unknown reference[%s] type=%s!",ref.name.data(),strDbType.data());
				continue;
			}
			ref.dw_desc = psubNode->GetAttribute("dw_desc");
			ref.cls_name = psubNode->GetAttribute("cls_name");
			//当时固定类型时
			if (ref.dbtype == "1")
			{
				int irowNode = psubNode->GetChildCount("row");
				for (int j = 0; j < irowNode; j++)
				{
					SBaseConfig *pRowNode = psubNode->GetChildNode(j, "row");
					if (pRowNode == NULL)
					{
						//delete pRow;
						continue;
					}
					myRow *pRow = new myRow();	
					pRow->id=pRowNode->GetAttribute("row_id");
					pRow->name=pRowNode->GetAttribute("row_name");
					ref.myRowList.append(pRow);
					ref.myRowList.setAutoDelete(true);
				}
			} else {
				//对sql语句中可能存在的转译字符进行转换
				ref.dw_select = psubNode->GetChildNode("select")->GetNodeValue();
				ref.dw_select.replace("&lt;=","<=");//转义
				ref.dw_select.replace("&gt;",">");//转义
				//ref.dw_select.replace("\'","\\\'");
				ref.dw_select.replace("'","''");
			}
			//对获取的引用节点进行处理
			if(!handleOneRefNode(curMaxdwsn, &ref))
			{
				//如果数据库操作失败，则应该停止后续操作
				break;
			}
		} else if (psubNode->GetNodeName() == "dataset")
		{
			myDataset curDataSet;
			curDataSet.name = psubNode->GetAttribute("name");
			SString strDbType=psubNode->GetAttribute("dbtype");
			if(strDbType == "db")
				curDataSet.dbtype = 2;
			else if(strDbType == "mdb")
				curDataSet.dbtype = 3;
			else
			{
				LOGWARN("Unknown reference[%s] type=%s!",curDataSet.name.data(),strDbType.data());
				continue;
			}
			curDataSet.dw_desc = psubNode->GetAttribute("dw_desc");
			//curDataSet.dw_select = psubNode->GetAttribute("name");
			curDataSet.transform_type = psubNode->GetAttribute("transform_type").toInt();
			curDataSet.cross_refname = psubNode->GetAttribute("cross_refname");
			curDataSet.cross_column = psubNode->GetAttribute("cross_column").toInt();
			curDataSet.cross_data_column = psubNode->GetAttribute("cross_data_column").toInt();
			curDataSet.cls_name = psubNode->GetAttribute("cls_name");
			curDataSet.ext_attr = psubNode->GetAttribute("ext_attr");
			SBaseConfig * pSubSelNode = psubNode->GetChildNode(0,"select");
			if (pSubSelNode == NULL)
			{
				continue;
			}
			curDataSet.dw_select = pSubSelNode->GetNodeValue();
			curDataSet.dw_select.replace("&lt;","<");//转义
			curDataSet.dw_select.replace("&gt;",">");//转义
			curDataSet.dw_select.replace("'","''");
			//获取列定义数据
			SBaseConfig * pSubColNode = psubNode->GetChildNode(0,"columns");
			if (pSubColNode != NULL)
			{
				int icolnum = pSubColNode->GetChildCount("col");
				for (int i = 0; i < icolnum; i++)
				{
					SBaseConfig *pNode = pSubColNode->GetChildNode(i, "col");
					if (pNode == NULL)
					{
						//delete pRow;
						continue;
					}
					myCol *pColSet = new myCol();
					pColSet->col_name = pNode->GetAttribute("col_name");
					pColSet->col_vtype = pNode->GetAttribute("col_vtype");
					pColSet->col_width = pNode->GetAttribute("col_width").toInt();
					pColSet->ref_name = pNode->GetAttribute("ref_name");
					pColSet->conv = pNode->GetAttribute("conv");
					pColSet->pkey = pNode->GetAttribute("pkey").toInt();
					pColSet->isnull = pNode->GetAttribute("isnull").toInt();
					pColSet->ukey=pNode->GetAttribute("ukey").toInt();
					pColSet->ext_attr=pNode->GetAttribute("ext_attr");
					curDataSet.myColList.append(pColSet);
					curDataSet.myColList.setAutoDelete(true);
				}
			}
			
			//获取条件定义数据
			SBaseConfig * pSubConditonNode = psubNode->GetChildNode(0,"condition");
			if (pSubConditonNode != NULL)
			{
				int iconditionnum = pSubConditonNode->GetChildCount("con");
				for (int i = 0; i < iconditionnum; i++)
				{
					SBaseConfig *pNode = pSubConditonNode->GetChildNode(i, "con");
					if (pNode == NULL)
					{
						//delete pRow;
						continue;
					}
					myCondition *pConditionSet = new myCondition();
					pConditionSet->con_name = pNode->GetAttribute("con_name");
					pConditionSet->is_option = pNode->GetAttribute("is_option").toInt();
					pConditionSet->cmp_type = pNode->GetAttribute("cmp_type").toInt();
					pConditionSet->input_type = pNode->GetAttribute("input_type").toInt();
					pConditionSet->ref_name = pNode->GetAttribute("ref_name");
					pConditionSet->def_value = pNode->GetAttribute("def_value");
					pConditionSet->dyn_where = pNode->GetAttribute("dyn_where");
					curDataSet.myConditionList.append(pConditionSet);
					curDataSet.myConditionList.setAutoDelete(true);
				}
			}
			
			if (!handleOneDataNode(curMaxdwsn,&curDataSet))
			{
				break;
			}
			
		}
	}
#endif
	
	refreshLeftTable();//刷新主表格
	SSP_GUI->HideWaiting();
}
//导出
void view_datawindow_edit::onPushButton_export()
{
	OutputCfg dlg(this);
	dlg.exec();

}
//应用
void view_datawindow_edit::onPushButton_apply()
{
	SSP_GUI->ShowWaiting();
	SQt::ProcessUiEvents();
	CSsp_DatawindowMgr *pOld = GET_DW_CFG;
	pOld->LoadByDb();
// 	CSsp_DatawindowMgr *pDw = new CSsp_DatawindowMgr();
// 	pDw->SetDbOper(DB);
// 	pDw->SetMdbOper(MDB);
// 	pDw->LoadByDb();
// 	CSsp_DatawindowMgr *pOld = GET_DW_CFG;
// 	CConfigMgr::RemoveConfig(pOld,true);
	SSP_GUI->HideWaiting();
}

//选择引用行的行删除可用
void view_datawindow_edit::ref2ItemCilcked(QTableWidgetItem *item,QTableWidgetItem *pre)
{
	S_UNUSED(item);
	S_UNUSED(pre);
	int row = ui.tableWidget_ref1->currentRow();
	if (row == -1)
	{
		return;
	}
	QString strDbType = ui.tableWidget_ref1->item(row,2)->text();
	if (strDbType != "固定")
	{
		ui.pushButton_ref2_del->setEnabled(false);
	} else {
		ui.pushButton_ref2_del->setEnabled(true);
	}
	
}
//选择数据集列的行删除可用
void view_datawindow_edit::data2ItemCilcked(QTableWidgetItem *item,QTableWidgetItem *pre)
{
	S_UNUSED(item);
	S_UNUSED(pre);
	ui.pushButton_data2_del->setEnabled(true);
}
//切换设置哪些按钮可用
void view_datawindow_edit::tabChanged(int num)
{
	if(num==0)
	{
		if(ui.tableWidget_ref1->currentRow()< 0)
		{
			ui.pushButton_copy->setEnabled(false);
			ui.pushButton_del->setEnabled(false);
			ui.pushButton_view->setEnabled(false);
			ui.pushButton_ref2_del->setEnabled(false);
		}
		else
		{
			ui.pushButton_copy->setEnabled(true);
			if (ui.tableWidget_ref1->currentRow() < 0)
			{
				ui.pushButton_del->setEnabled(false);
			} else {
				ui.pushButton_del->setEnabled(true);
			}
			//ui.pushButton_del->setEnabled(true);
			ui.pushButton_view->setEnabled(false);
			if (ui.tableWidget_ref2->currentRow() < 0)
			{
				ui.pushButton_ref2_del->setEnabled(false);
			} else {
				ui.pushButton_ref2_del->setEnabled(true);
			}
		}
		
	}
	else if(num==1)
	{
		if(ui.tableWidget_data1->currentRow() < 0)
		{
			ui.pushButton_copy->setEnabled(false);
			ui.pushButton_del->setEnabled(false);
			ui.pushButton_view->setEnabled(false);
			ui.pushButton_data2_del->setEnabled(false);
		}
		else
		{
			ui.pushButton_copy->setEnabled(true);
			if (ui.tableWidget_data1->currentRow() < 0)
			{
				ui.pushButton_del->setEnabled(false);
				ui.pushButton_view->setEnabled(false);
			} else {
				ui.pushButton_del->setEnabled(true);
				ui.pushButton_view->setEnabled(true);
			}
		
			//ui.pushButton_view->setEnabled(true);
			if(ui.tableWidget_data2->currentRow()< 0)
			{
				ui.pushButton_data2_del->setEnabled(false);
			} else {
				ui.pushButton_data2_del->setEnabled(true);
			}
			viewDwConEdit->refreshDataRef();			
		}
		////if(ui.tableWidget_data2->currentRow()<=0)
		//{
		//	ui.pushButton_data2_del->setEnabled(false);
		//}
	}
}
//点击左侧引用主表在右侧显示sql和行值
void view_datawindow_edit::showRefInfo(QTableWidgetItem *item,QTableWidgetItem *pre)
{
	S_UNUSED(pre);
	setRefBlockSignals(true);

	m_rsLeftTable.clear();
	m_rsRow.clear();
	//ui.tableWidget_ref2->clear();
	ui.textEdit_ref->clear();
	ui.tableWidget_ref2->clearContents();
	ui.tableWidget_ref2->setRowCount(0);

	ui.pushButton_copy->setEnabled(true);
	ui.pushButton_del->setEnabled(true);
	ui.pushButton_view->setEnabled(false);
	ui.pushButton_ref2_del->setEnabled(false);
	//ui.pushButton_ref2_add->setEnabled(true);
	//ui.pushButton_ref2_del->setEnabled(true);

	SString sn=ui.tableWidget_ref1->item(item->row(),ref1_sn)->text().toStdString().data();
	QString dbType=ui.tableWidget_ref1->item(item->row(),2)->text();
	if (dbType == "固定")
	{
		ui.pushButton_ref2_add->setEnabled(true);
	} else {
		ui.pushButton_ref2_add->setEnabled(false);
	}

	ui.pushButton_ref2_del->setEnabled(false);

	SString sql = SString::toFormat("select dw_sn,dstype,name,dbtype,dw_desc,dw_select from t_ssp_data_window where dstype=1 and dw_sn=%s order by cls_name desc,dw_desc",sn.data());
	int ret = pDb->RetrieveRecordset(sql, m_rsLeftTable);
	if(ret < 0)
	{
		QMessageBox::information(this,tr("警告"),tr("读取在数据库失败[%1]").arg(sql.data()),QMessageBox::Ok);
		setRefBlockSignals(false);
		return;
	} else if (ret == 0)
	{
		setRefBlockSignals(false);
		return;
	} else if (ret > 1)
	{
		QMessageBox::information(this,tr("警告"),tr("数据库存在多条"),QMessageBox::Ok);
		setRefBlockSignals(false);
		return;
	}

	ui.textEdit_ref->setText(m_rsLeftTable.GetValue(0,5).data());
	if (dbType == "固定" )
	{
		sql = SString::toFormat("select dw_sn,row_sn,row_id,row_name from t_ssp_data_window_row where dw_sn=%s order by row_sn ",sn.data());
		ret = pDb->RetrieveRecordset(sql, m_rsRow);
		if(ret < 0)
		{
			QMessageBox::information(this,tr("警告"),tr("读取在数据库引用行失败[%1]").arg(sql.data()),QMessageBox::Ok);
			setRefBlockSignals(false);
			return;
		}else if (ret == 0)
		{
			setRefBlockSignals(false);
			return;
		}

		for(int i=0;i<m_rsRow.GetRows();i++)
		{
			ui.tableWidget_ref2->insertRow(i);
			QTableWidgetItem *item=new QTableWidgetItem(m_rsRow.GetValue(i,3).data());//名称
			ui.tableWidget_ref2->setItem(i,1,item);

			item=new QTableWidgetItem(m_rsRow.GetValue(i,2).data());
			ui.tableWidget_ref2->setItem(i,0,item);//序号

			item=new QTableWidgetItem(m_rsRow.GetValue(i,0).data());
			ui.tableWidget_ref2->setItem(i,2,item);//sn

			item=new QTableWidgetItem(m_rsRow.GetValue(i,1).data());
			ui.tableWidget_ref2->setItem(i,3,item);//row_sn
		}
		//针对“固定”类型table表可编译
		ui.tableWidget_ref2->setEditTriggers(QAbstractItemView::DoubleClicked);		//可编辑
	} else 
	{
		SRecordset rs;
		//SDatabaseOper *pDb = DB;
		//QStringList header;
		sql = m_rsLeftTable.GetValue(0,5).data();
		bool bNeedReplace = true;
		while (bNeedReplace)
		{
			int iPosStart = sql.find("{");
			if (iPosStart >= 0)
			{
				int iPosEnd =  sql.find("}");
				SString strSub = sql.substr(iPosStart,iPosEnd-iPosStart+1);
				sql.replace(strSub,"");
			} else {
				bNeedReplace = false;
				break;
			}
		}
		ret = pDb->RetrieveRecordset(sql,rs);
		if (ret < 0)
		{
			QMessageBox::information(this,tr("警告"),tr("读取在数据库引用行失败[%1]").arg(sql.data()),QMessageBox::Ok);
			setRefBlockSignals(false);
			return;
		}else if (ret == 0)
		{
			setRefBlockSignals(false);
			return;
		}
		for(int i=0; i < rs.GetRows();i++)
		{
			ui.tableWidget_ref2->insertRow(i);

			QTableWidgetItem *item=new QTableWidgetItem(rs.GetValue(i,0).data());
			item->setFlags(item->flags()&~Qt::ItemIsEditable);
			ui.tableWidget_ref2->setItem(i,0,item);

			item=new QTableWidgetItem(rs.GetValue(i,1).data());
			ui.tableWidget_ref2->setItem(i,1,item);
		}
		ui.tableWidget_ref2->setEditTriggers(QAbstractItemView::NoEditTriggers);		//不可编辑
	}

	setRefBlockSignals(false);
	
	ui.tableWidget_ref2->resizeColumnsToContents();
	if (ui.tableWidget_ref2->rowCount() > 0)
	{
		ui.tableWidget_ref2->selectRow(0);
		ui.pushButton_ref2_del->setEnabled(true);
	} else {
		ui.pushButton_ref2_del->setEnabled(false);
	}
}
//点击左侧数据集主表在右侧显示sql和列值
void view_datawindow_edit::showDataInfo(QTableWidgetItem *item,QTableWidgetItem *pre)
{
	S_UNUSED(pre);
	if (item == NULL)
	{
		return;
	}
	int curSelRow = item->row();
	if (curSelRow < 0)
	{
		QMessageBox::information(this,tr("警告"),tr("未选中主表对象，请选中主表对象！"),QMessageBox::Ok);
		return;
	}
	setDataBlockSignals(true);
	m_rsLeftTable.clear();
	m_rsCol.clear();

	ui.textEdit_data->clear();
	ui.tableWidget_data2->clearContents();
	ui.tableWidget_data2->setRowCount(0);
	ui.pushButton_data2_del->setEnabled(false);
	SString sn=ui.tableWidget_data1->item(curSelRow,COL_DATA1_SN)->text().toStdString().data();

	SString sql = SString::toFormat("select dw_sn,dstype,name,dbtype,dw_desc,dw_select from t_ssp_data_window where dstype=2 and dw_sn=%s order by cls_name desc,dw_desc",sn.data());
	int ret = pDb->RetrieveRecordset(sql, m_rsLeftTable);
	if(ret < 0)
	{
		QMessageBox::information(this,tr("警告"),tr("读取数据库失败[%1]").arg(sql.data()),QMessageBox::Ok);
		setDataBlockSignals(false);
		return;
	} else if (ret == 0)
	{
		setDataBlockSignals(false);
		return;
	} else if (ret > 1)
	{
		QMessageBox::information(this,tr("警告"),tr("数据库存在多条"),QMessageBox::Ok);
		setDataBlockSignals(false);
		return;
	}
	QString strDwSelect = m_rsLeftTable.GetValue(0,5).data();
	ui.textEdit_data->setText(strDwSelect);
	QString strComboText = ui.comboBox_data_cls->currentText();

	sql = SString::toFormat("select dw_sn,col_sn,col_name,col_vtype,col_width,ref_name,conv,pkey,isnull,ukey,ext_attr from t_ssp_data_window_col where dw_sn=%s order by col_sn",sn.data());
	ret = pDb->RetrieveRecordset(sql,m_rsCol);
	if(ret < 0)
	{
		QMessageBox::information(this,tr("警告"),tr("数据库查询失败[%1]！").arg(sql.data()),QMessageBox::Ok);
		setDataBlockSignals(false);
		return;
	} else if ( ret == 0)
	{
		setDataBlockSignals(false);
		return;
	}


	SRecordset refNameSets;
	pDb->RetrieveRecordset(SString::toFormat("select name,dw_desc from t_ssp_data_window where dstype=1 order by cls_name desc,dw_desc"),refNameSets);

	for(int i=0;i<m_rsCol.GetRows();i++)
	{
 		ui.tableWidget_data2->insertRow(i);
		item=new QTableWidgetItem(m_rsCol.GetValue(i,1).data());
		//item->setFlags(item->flags()&~Qt::ItemIsEditable);
		ui.tableWidget_data2->setItem(i,0,item);//序号

		QTableWidgetItem *item=new QTableWidgetItem(valuetypeTransform(m_rsCol.GetValue(i,3)).data());
		ui.tableWidget_data2->setItem(i,2,item);//类型

		item=new QTableWidgetItem(m_rsCol.GetValue(i,2).data());
		ui.tableWidget_data2->setItem(i,1,item);//名称

		item=new QTableWidgetItem();
		item->setCheckState(Qt::Unchecked);
		if(m_rsCol.GetValue(i,7)=="1")
			item->setCheckState(Qt::Checked);
		item->setFlags(item->flags()&~Qt::ItemIsEditable);
		ui.tableWidget_data2->setItem(i,3,item);//主键

		item=new QTableWidgetItem(m_rsCol.GetValue(i,4).data());
		ui.tableWidget_data2->setItem(i,4,item);//宽度

		item=new QTableWidgetItem(refnameTransform(m_rsCol.GetValue(i,5),refNameSets).data());
		ui.tableWidget_data2->setItem(i,5,item);//引用名称

		item=new QTableWidgetItem(convStrToName(m_rsCol.GetValue(i,6)).data());
		ui.tableWidget_data2->setItem(i,6,item);//转换方法
		item=new QTableWidgetItem(m_rsCol.GetValue(i,10).data());
		ui.tableWidget_data2->setItem(i,7,item);//ext_attr
		item=new QTableWidgetItem(m_rsCol.GetValue(i,1).data());
		ui.tableWidget_data2->setItem(i,8,item);//
		item=new QTableWidgetItem(m_rsCol.GetValue(i,0).data());
		ui.tableWidget_data2->setItem(i,COL_DATA2_SN,item);//		
	}
	setDataBlockSignals(false);
	ui.tableWidget_data2->resizeColumnsToContents();
	if (ui.tableWidget_data2->rowCount() > 0)
	{
		ui.tableWidget_data2->selectRow(0);
		ui.pushButton_data2_del->setEnabled(true);
	}else{
		ui.pushButton_data2_del->setEnabled(false);
	}
}
void view_datawindow_edit::showConInfo(QTableWidgetItem *item,QTableWidgetItem *pre)
{
	S_UNUSED(pre);
	if (item == NULL)
	{
		return;
	}
	int row=item->row();
	if (row < 0)
	{
		return;
	}
	QString strComBoxText = ui.comboBox_data_cls->currentText();
	bool bCheckState = ui.checkBox_useCls->isChecked() ? true:false;
	viewDwConEdit->refreshTable(ui.tableWidget_data1->item(row,COL_DATA1_SN)->text().toInt(),strComBoxText,bCheckState);
}
//编辑主表信息保存
void view_datawindow_edit::editRef1(QTableWidgetItem *item)
{
	//ui.tableWidget_ref1->blockSignals(true);
 	int col=item->column();
	int row=item->row();
	bool flag =true;
	if(col==3)
	{
		flag=pDb->Execute(SString::toFormat("update t_ssp_data_window set dw_desc='%s' where dw_sn=%s",\
			ui.tableWidget_ref1->item(row,3)->text().toStdString().c_str(),ui.tableWidget_ref1->item(row,ref1_sn)->text().toStdString().c_str()));
	}
	else if(col==2)
	{
		flag=pDb->Execute(SString::toFormat("update t_ssp_data_window set dbtype=%s where dw_sn=%s",\
			typeTransform(ui.tableWidget_ref1->item(row,2)->text().toStdString().data()).data(),ui.tableWidget_ref1->item(row,ref1_sn)->text().toStdString().c_str()));
	}
	else if(col==1)
	{
		//判断下当前的名称是否存在重复情况
		SRecordset tmpset;
		SString sql = SString::toFormat("select name from t_ssp_data_window where name='%s' and dstype=1",
			ui.tableWidget_ref1->item(row,1)->text().toStdString().c_str());
		int ret = pDb->RetrieveRecordset(sql,tmpset);
		if (ret < 0)
		{
			QMessageBox::warning(this,tr("警告"),tr("在数据库查询失败【%1】").arg(sql.data()),QMessageBox::Ok);
			return;
		} else if (ret > 0)
		{
			QMessageBox::warning(this,tr("警告"),tr("当前名称重复，请重新输入！"),QMessageBox::Ok);
			return;
		}
		flag=pDb->Execute(SString::toFormat("update t_ssp_data_window set name='%s' where dw_sn=%s",\
			ui.tableWidget_ref1->item(row,1)->text().toStdString().c_str(),ui.tableWidget_ref1->item(row,ref1_sn)->text().toStdString().c_str()));
	}
	else if(col==0)
	{
		flag=pDb->Execute(SString::toFormat("update t_ssp_data_window set cls_name='%s' where dw_sn=%s",\
			ui.tableWidget_ref1->item(row,0)->text().toStdString().c_str(),ui.tableWidget_ref1->item(row,ref1_sn)->text().toStdString().c_str()));
		if(flag==false)
		{
			QMessageBox::warning(this,tr("警告"),tr("在数据库更新引用失败"),QMessageBox::Ok);
			//ui.tableWidget_ref1->blockSignals(false);
			return;
		}
		//这里保持引用页原来combo选择类型不变
		QString strCurRefComboText = ui.comboBox_ref_cls->currentText();
		initRefCombo();
		ui.comboBox_ref_cls->blockSignals(true);
		ui.comboBox_ref_cls->setEditText(strCurRefComboText);
		ui.comboBox_ref_cls->blockSignals(false);
	}
	if(flag==false)
	{
		QMessageBox::warning(this,tr("警告"),tr("在数据库更新引用失败"),QMessageBox::Ok);
		//ui.tableWidget_ref1->blockSignals(false);
		return;
	}
	//ui.tableWidget_ref1->blockSignals(false);
}
//编辑引用行信息保存
void view_datawindow_edit::editRef2(QTableWidgetItem *item)
{
	ui.tableWidget_ref2->blockSignals(true);
	int col=item->column();
	int row=item->row();
	QString strDwsn=ui.tableWidget_ref2->item(row,ref2_sn)->text();
	QString strRowsn = ui.tableWidget_ref2->item(row,COL_REF2_ROW_SN)->text();

	SString sql;
	if(col==0)
	{
		sql = SString::toFormat("update t_ssp_data_window_row set row_id='%s' where dw_sn=%s and row_sn=%s",
			ui.tableWidget_ref2->item(row,0)->text().toStdString().c_str(),strDwsn.toStdString().c_str(),strRowsn.toStdString().c_str());
	}
	if(col==1)
	{
		sql = SString::toFormat("update t_ssp_data_window_row set row_name='%s' where dw_sn=%s and row_sn=%s",
			ui.tableWidget_ref2->item(row,1)->text().toStdString().c_str(),strDwsn.toStdString().c_str(), strRowsn.toStdString().c_str());
	}
	bool flag=pDb->Execute(sql);
	if(flag==false)
	{
		QMessageBox::warning(this,tr("警告"),tr("在数据库更新引用对应行失败[%1]").arg(sql.data()),QMessageBox::Ok);
		ui.tableWidget_ref2->blockSignals(false);
		return;
	}
	ui.tableWidget_ref2->blockSignals(false);
}
//编辑数据集信息保存
void view_datawindow_edit::editData1(QTableWidgetItem *item)
{
	int col=item->column();
	int row=item->row();
	//ui.tableWidget_data1->blockSignals(true);
	SString sql;
	if(col==0)
	{
		sql = SString::toFormat("update t_ssp_data_window set cls_name='%s' where dw_sn=%s",\
			ui.tableWidget_data1->item(row,0)->text().toStdString().c_str(),ui.tableWidget_data1->item(row,data1_sn)->text().toStdString().data());
		bool flag=pDb->Execute(sql);
		if(flag==false)
		{
			QMessageBox::warning(this,tr("警告"),tr("在数据库更新数据集失败[%1]").arg(sql.data()),QMessageBox::Ok);
			//ui.tableWidget_data1->blockSignals(false);
			return;
		}

		//这里保持引用页原来combo选择类型不变
		QString strCurDataComboText = ui.comboBox_data_cls->currentText();
		initDataCombo();
		if (strCurDataComboText != "全部")
		{
			ui.comboBox_ref_cls->setEditText(strCurDataComboText);
		}  else 
		{
			ui.comboBox_ref_cls->blockSignals(true);
			ui.comboBox_ref_cls->setEditText(strCurDataComboText);
			ui.comboBox_ref_cls->blockSignals(false);
		}
		return;
		
	}
	if(col==1)
	{
		SRecordset tmpset;
		sql = SString::toFormat("select name from t_ssp_data_window where name='%s' and dstype=2",
			ui.tableWidget_data1->item(row,1)->text().toStdString().c_str());
		int ret = pDb->RetrieveRecordset(sql,tmpset);
		if (ret < 0)
		{
			QMessageBox::warning(this,tr("警告"),tr("在数据库查询失败【%1】").arg(sql.data()),QMessageBox::Ok);
			return;
		} else if (ret > 0)
		{
			QMessageBox::warning(this,tr("警告"),tr("当前名称重复，请重新输入！"),QMessageBox::Ok);
			return;
		}
		sql = SString::toFormat("update t_ssp_data_window set name='%s' where dw_sn=%s",\
			ui.tableWidget_data1->item(row,1)->text().toStdString().c_str(),ui.tableWidget_data1->item(row,data1_sn)->text().toStdString().data());
	}
	if(col==2)
	{
		sql = SString::toFormat("update t_ssp_data_window set dbtype=%s where dw_sn=%s",\
			typeTransform(ui.tableWidget_data1->item(row,2)->text().toStdString().data()).data(),ui.tableWidget_data1->item(row,data1_sn)->text().toStdString().data());
	}
	if(col==3)
	{
		sql = SString::toFormat("update t_ssp_data_window set dw_desc='%s' where dw_sn=%s",\
			ui.tableWidget_data1->item(row,3)->text().toStdString().c_str(),ui.tableWidget_data1->item(row,data1_sn)->text().toStdString().data());
	}

	if(col==4)
	{
		sql = SString::toFormat("update t_ssp_data_window set transform_type=%s where dw_sn=%s",\
			tranftranftype(ui.tableWidget_data1->item(row,col)->text().toStdString().data()).data(),ui.tableWidget_data1->item(row,data1_sn)->text().toStdString().data());
	}

	if(col==5)
	{
		sql = SString::toFormat("update t_ssp_data_window set cross_refname='%s' where dw_sn=%s",\
			ui.tableWidget_data1->item(row,col)->text().toStdString().c_str(),ui.tableWidget_data1->item(row,data1_sn)->text().toStdString().data());
	}

	if(col==6)
	{
		sql = SString::toFormat("update t_ssp_data_window set cross_column=%s where dw_sn=%s",\
			ui.tableWidget_data1->item(row,col)->text().toStdString().c_str(),ui.tableWidget_data1->item(row,data1_sn)->text().toStdString().data());
	}

	if(col==7)
	{
		sql = SString::toFormat("update t_ssp_data_window set cross_data_column=%s where dw_sn=%s",\
			ui.tableWidget_data1->item(row,col)->text().toStdString().c_str(),ui.tableWidget_data1->item(row,data1_sn)->text().toStdString().data());
	}
	if(col==8)
	{
		sql = SString::toFormat("update t_ssp_data_window set ext_attr='%s' where dw_sn=%s",\
			ui.tableWidget_data1->item(row,col)->text().toStdString().c_str(),ui.tableWidget_data1->item(row,data1_sn)->text().toStdString().data());
		
	}	
	bool flag=pDb->Execute(sql);
	if(flag==false)
	{
		QMessageBox::warning(this,tr("警告"),tr("在数据库更新数据集失败[%1]").arg(sql.data()),QMessageBox::Ok);
		ui.tableWidget_data1->blockSignals(false);
		return;
	}
	ui.tableWidget_data1->resizeColumnsToContents();
}
//编辑数据集列信息保存
void view_datawindow_edit::editData2(QTableWidgetItem *item)
{
	QString strTest = item->text();
	int col=item->column();
	int row=item->row();
	ui.tableWidget_data2->blockSignals(true);
	QString sn=ui.tableWidget_data2->item(row,COL_DATA2_SN)->text();
	SRecordset sets;
	SString sql;
	if(col==0)//序号
	{
		//特别处理，需要判断当前修改的序号是否存在冲突
		QString oldno=ui.tableWidget_data2->item(row,8)->text();
		sql = SString::toFormat("select * from t_ssp_data_window_col where dw_sn=%s and col_sn=%s",ui.tableWidget_data2->item(row,COL_DATA2_SN)->text().toStdString().data(),item->text().toStdString().c_str());
		SRecordset tempset;
		int ret = pDb->RetrieveRecordset(sql,tempset);
		if (ret < 0)
		{
			QMessageBox::warning(this,tr("警告"),tr("查询数据库数据集失败[%1]").arg(sql.data()),QMessageBox::Ok);
			ui.tableWidget_data2->item(row,0)->setText(oldno);
			ui.tableWidget_data2->blockSignals(false);
			return;
		} else if ( ret > 0)
		{
			QMessageBox::warning(this,tr("警告"),tr("当前新序号存在冲突，请重新修改！"),QMessageBox::Ok);
			ui.tableWidget_data2->item(row,0)->setText(oldno);
			ui.tableWidget_data2->blockSignals(false);
			return;
		}
		sql = SString::toFormat("update t_ssp_data_window_col set col_sn=%d where dw_sn=%s and col_sn=%s",ui.tableWidget_data2->item(row,0)->text().toInt(),ui.tableWidget_data2->item(row,COL_DATA2_SN)->text().toStdString().data(),oldno.toStdString().data());
		bool flag=pDb->Execute(sql);
		if(flag==false)
		{
			QMessageBox::warning(this,tr("警告"),tr("在数据库更新数据集自有列失败[%1]").arg(sql.data()),QMessageBox::Ok);
			ui.tableWidget_data2->item(row,0)->setText(oldno);
			ui.tableWidget_data2->blockSignals(false);
			return;
		}
		ui.tableWidget_data2->item(row,8)->setText(ui.tableWidget_data2->item(row,0)->text());
	}

	else if(col==2)//类型
	{
		QString name=ui.tableWidget_data2->item(row,2)->text();
		sql = SString::toFormat("update t_ssp_data_window_col set col_vtype='%s' where dw_sn=%s and col_sn=%s",\
			valuetypeTransform(ui.tableWidget_data2->item(row,2)->text().toStdString().data()).data(),ui.tableWidget_data2->item(row,COL_DATA2_SN)->text().toStdString().data(),ui.tableWidget_data2->item(row,0)->text().toStdString().data());
	}
	else if(col==1)//名称
	{
		sql = SString::toFormat("update t_ssp_data_window_col set col_name='%s' where dw_sn=%s and col_sn=%s",\
			ui.tableWidget_data2->item(row,1)->text().toStdString().c_str(),ui.tableWidget_data2->item(row,COL_DATA2_SN)->text().toStdString().c_str(),ui.tableWidget_data2->item(row,0)->text().toStdString().data());
	}
	else if(col==3)//主键
	{
		const char* mkey="0";
		if(ui.tableWidget_data2->item(row,3)->checkState()==Qt::Checked)
			mkey="1";
		QString name=ui.tableWidget_data2->item(row,data2_sn)->text();
		sql = SString::toFormat("update t_ssp_data_window_col set pkey=%s where dw_sn=%s and col_sn=%s",\
			mkey,ui.tableWidget_data2->item(row,data2_sn)->text().toStdString().data(),ui.tableWidget_data2->item(row,0)->text().toStdString().data());
	}
	else if(col==4)//宽度
	{
		QString strColWidth = ui.tableWidget_data2->item(row,4)->text();
		strColWidth = strColWidth.isEmpty() ? "0":strColWidth;
		sql = SString::toFormat("update t_ssp_data_window_col set col_width=%s where dw_sn=%s and col_sn=%s",\
			strColWidth.toStdString().c_str(),ui.tableWidget_data2->item(row,data2_sn)->text().toStdString().data(),ui.tableWidget_data2->item(row,0)->text().toStdString().data());
		ui.tableWidget_data2->item(row,4)->setText(strColWidth);
	}

	else if(col==5)//引用名
	{
		SRecordset refNameSets;
		pDb->RetrieveRecordset(SString::toFormat("select name,dw_desc from t_ssp_data_window where dstype=1 order by cls_name desc,dw_desc"),refNameSets);
		sql = SString::toFormat("update t_ssp_data_window_col set ref_name='%s' where dw_sn=%s and col_sn=%s",\
			refnameTransform(ui.tableWidget_data2->item(row,5)->text().toStdString().data(),refNameSets).data(),ui.tableWidget_data2->item(row,data2_sn)->text().toStdString().data(),ui.tableWidget_data2->item(row,0)->text().toStdString().data());
	}

	else if(col==6)//转换方法
	{
		sql = SString::toFormat("update t_ssp_data_window_col set conv='%s' where dw_sn=%s and col_sn=%s",\
			convNameToStr(ui.tableWidget_data2->item(row,6)->text().toStdString().data()).data(),ui.tableWidget_data2->item(row,data2_sn)->text().toStdString().data(),ui.tableWidget_data2->item(row,0)->text().toStdString().data());
	}

	else if(col==7)//扩展属性
	{
		QString tmp1=ui.tableWidget_data2->item(row,7)->text();
		QString tmp2=ui.tableWidget_data2->item(row,data2_sn)->text();
		QString tmp3=ui.tableWidget_data2->item(row,0)->text();
		sql = SString::toFormat("update t_ssp_data_window_col set ext_attr='%s' where dw_sn=%s and col_sn=%s",\
			ui.tableWidget_data2->item(row,7)->text().toStdString().c_str(),ui.tableWidget_data2->item(row,data2_sn)->text().toStdString().c_str(),ui.tableWidget_data2->item(row,0)->text().toStdString().data());
	}
	bool flag=pDb->Execute(sql);
	if(flag==false)
	{
		QMessageBox::warning(this,tr("警告"),tr("在数据库更新失败[%1]").arg(sql.data()),QMessageBox::Ok);
		ui.tableWidget_data2->blockSignals(false);
		return;
	}
	ui.tableWidget_data2->blockSignals(false);
	ui.tableWidget_data2->resizeColumnsToContents();
}
//引用sql修改
void view_datawindow_edit::editRefText()
{
	int curRefRow=ui.tableWidget_ref1->currentRow();
	int type = pDb->SelectIntoI(SString::toFormat("select dbtype from t_ssp_data_window where dw_sn=%s",
		ui.tableWidget_ref1->item(curRefRow,ref1_sn)->text().toStdString().c_str()));
	if(type == 1)
		return;

// 	m_rsLeftTable.clear();
// 	pDb->RetrieveRecordset("select dw_sn,dstype,name,dbtype,dw_desc,dw_select from t_ssp_data_window order by cls_name desc,dw_desc",m_rsLeftTable);
// 	
// 	if(m_rsLeftTable.GetValue(curRefRow,3)=="1")
// 		return;
	QString str=ui.textEdit_ref->toPlainText();
	str.replace("'","''");
	/*bool flag=*/pDb->Execute(SString::toFormat("update t_ssp_data_window set dw_select='%s' where dw_sn=%s",\
		str.toStdString().c_str(),ui.tableWidget_ref1->item(curRefRow,ref1_sn)->text().toStdString().c_str()));
// 	if(flag==false)
// 	{
// 		QMessageBox::warning(this,tr("警告"),tr("在数据库更新引用SQL语句失败"),QMessageBox::Ok);
// 		return;
// 	}
}
//数据集sql修改
void view_datawindow_edit::editDataText()
{
	int curdataRow=ui.tableWidget_data1->currentRow();
	QString str=ui.textEdit_data->toPlainText();
	str.replace("'","''");
	/*bool flag=*/pDb->Execute(SString::toFormat("update t_ssp_data_window set dw_select='%s' where dw_sn=%s",\
		str.toStdString().c_str(),ui.tableWidget_data1->item(curdataRow,data1_sn)->text().toStdString().data()));
// 	if(flag==false)
// 	{
// 		QMessageBox::warning(this,tr("警告"),tr("在数据库更新数据集SQL语句失败"),QMessageBox::Ok);
// 		return;
// 	}

}
//检查是否为空
char* view_datawindow_edit::checkNull(char* num)
{
	if(strlen(num)==0)
		num=(char*)"0";
	return num;	
	if(SString::equals((const char*)num,"false"))
		return (char*)"0";
	if(SString::equals((const char*)num,"true"))
		return (char*)"1";
}

//输入转换
SString view_datawindow_edit::refnameTransform(SString str,SRecordset &datasets )
{
	if(datasets.GetColumns()!=2)
		return "";
	if(str=="")
		return "";
	for(int i=0;i<datasets.GetRows();i++)
	{
		if(str==datasets.GetValue(i,0))
			return datasets.GetValue(i,1);
		else if(str==datasets.GetValue(i,1))
			return datasets.GetValue(i,0);
	}
	return "";
}
SString view_datawindow_edit::convStrToName(SString str)//输入转换
{
	if(str=="")
		return "";
	else if(str=="soc2time")
		return ("世纪秒转为日期时间(yyyy-MM-dd hh:mm:ss)");
	else if(str=="soc2time2")
		return ("世纪秒转为日期时间(yyyy年MM月dd日 hh时mm分ss秒)");
	else if(str=="soc2time3")
		return ("世纪秒转为日期时间(hh:mm:ss)");
	else if(str=="soc2time4")
		return ("世纪秒转为日期时间(hh时mm分ss秒)");
	else if(str=="soc2date")
		return ("世纪秒转为日期时间(yyyy-MM-dd)");
	else if(str=="soc2date2")
		return ("世纪秒转为日期时间(yyyy年MM月dd日)");
	else if(str=="soc2now")
		return ("指定时间到现在时间的差");
	else if(str=="int2intk")
		return ("整数转为1024的倍数表示的缩写值");
	else if(str=="int2thousands")
		return ("整数改为千位间隔数");
	else if(str=="int2ip")
		return ("整数转换为IP地址字符串");
	return "";
}
SString view_datawindow_edit::convNameToStr(SString name)//输入转换
{
	if(name=="")
		return "";
	else if(name==("世纪秒转为日期时间(yyyy-MM-dd hh:mm:ss)"))
		return "soc2time";
	else if(name==("世纪秒转为日期时间(yyyy年MM月dd日 hh时mm分ss秒)"))
		return "soc2time2";
	else if(name==("世纪秒转为日期时间(hh:mm:ss)"))
		return "soc2time3";
	else if(name==("世纪秒转为日期时间(hh时mm分ss秒)"))
		return "soc2time4";
	else if(name==("世纪秒转为日期时间(yyyy-MM-dd)"))
		return "soc2date";
	else if(name==("世纪秒转为日期时间(yyyy年MM月dd日)"))
		return "soc2date2";
	else if(name=="指定时间到现在时间的差")
		return ("soc2now");
	else if(name==("整数转为1024的倍数表示的缩写值"))
		return "int2intk";
	else if(name==("整数改为千位间隔数"))
		return "int2thousands";
	else if(name==("整数转换为IP地址字符串"))
		return "int2ip";
	return "";
}
//输入转换
SString view_datawindow_edit::valuetypeTransform(SString str)
{
	if(str=="string")
		return "字符型";
	if(str=="float")
		return "浮点型";
	if(str=="int")
		return "整数型";
	if(str=="字符型")
		return "string";
	if(str=="浮点型")
		return "float";
	if(str=="整数型")
		return "int";
	return "";
}
SString view_datawindow_edit::tranftranftype(SString str)
{
	if(str=="0")
		return "不变换";
	if(str=="1")
		return "带标题行转置";
	if(str=="2")
		return "数据行转置";
	if(str=="3")
		return "交叉变换";
	if(str=="不变换")
		return "0";
	if(str=="带标题行转置")
		return "1";
	if(str=="数据行转置")
		return "2";
	if(str=="交叉变换")
		return "3";
	return "";
}
//输入转换
SString view_datawindow_edit::typeTransform(SString str)
{
	if(str=="1")
		return "固定";
	if(str=="2")
		return "历史库";
	if(str=="3")
		return "内存库";
	if(str=="fixed")
		return "1";
	if(str=="db")
		return "2";
	if(str=="mdb")
		return "3";
	if(str=="固定")
		return "1";
	if(str=="历史库")
		return "2";
	if(str=="内存库")
		return "3";
	return "";
}

void view_datawindow_edit::slotOnCheckBoxClick()
{
	int row = ui.tableWidget_data1->currentRow();
	if (row < 0)
	{
		QMessageBox::warning(this,tr("警告"),tr("当前数据集对象无选择行，请先选中行！"),QMessageBox::Ok);
		return;
	}

	QTableWidgetItem* item = ui.tableWidget_data1->item(row,0);
	if (item)
	{
		QString strComBoxText = ui.comboBox_data_cls->currentText();
		bool bCheckState = ui.checkBox_useCls->isChecked();
		viewDwConEdit->setComboTextAndCheckState(strComBoxText,bCheckState);
	}
		
}
//////////////////////////////////////////////////////////////////////

dataWindowDelegate::dataWindowDelegate(QObject *parent)
	: QStyledItemDelegate(parent)
{
	this->parent=static_cast<view_datawindow_edit*>(parent);
	pDb=DB;
}



QWidget* dataWindowDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	switch(index.column())
	{
	case 2:
		{
			QComboBox* editor = new QComboBox(parent);
			editor->insertItem(0,"字符型");
			editor->insertItem(1,"浮点型");
			editor->insertItem(2,"整数型");
			return editor;
		}
		break;
	case 5:
		{
			SRecordset m_rsLeftTable;
			QString strComText;
			bool bIsChecked;
			SString sql;
			this->parent->getComboxAndCheckState(strComText,bIsChecked);
			if (strComText != "全部" && bIsChecked)
			{
				sql = SString::toFormat("select dw_desc from t_ssp_data_window where dstype=1 and cls_name='%s' order by cls_name desc,dw_desc",strComText.toStdString().c_str());
			} else {
				sql = "select dw_desc from t_ssp_data_window where dstype=1 and dw_desc is not null order by cls_name desc,dw_desc";
			}

			pDb->RetrieveRecordset(sql,m_rsLeftTable);
			QComboBox* editor = new QComboBox(parent);
			editor->insertItem(0,"");
			for(int k=0;k<m_rsLeftTable.GetRows();k++)
			{
				editor->insertItem(k+1,m_rsLeftTable.GetValue(k,0).data());
			}
			return editor;
		}
		break;
	case 6:
		{
			QComboBox* editor = new QComboBox(parent);
			editor->insertItem(0,"");
			editor->insertItem(1,"世纪秒转为日期时间(yyyy-MM-dd hh:mm:ss)");
			editor->insertItem(2,"世纪秒转为日期时间(yyyy年MM月dd日 hh时mm分ss秒)");
			editor->insertItem(3,"世纪秒转为日期时间(hh:mm:ss)");
			editor->insertItem(4,"世纪秒转为日期时间(hh时mm分ss秒)");
			editor->insertItem(5,"世纪秒转为日期时间(yyyy-MM-dd)");
			editor->insertItem(6,"指定时间到现在时间的差");
			editor->insertItem(7,"世纪秒转为日期时间(yyyy年MM月dd日)");
			editor->insertItem(8,"整数转为1024的倍数表示的缩写值");
			editor->insertItem(9,"整数改为千位间隔数");
			editor->insertItem(10,"整数转换为IP地址字符串");
			return editor;
		}
		break;
	}
	QWidget *widget=QStyledItemDelegate::createEditor(parent,option,index);
	return widget;
}

void dataWindowDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
	QComboBox* comboEditor = qobject_cast<QComboBox* >(editor);
	if (comboEditor)
	{		
		QString text = index.model()->data(index,Qt::EditRole).toString();
		connect(comboEditor,SIGNAL(currentIndexChanged(QString)),this,SLOT(comboItemChanged(QString)));
		int index1 = comboEditor->findText(text);	

		comboEditor->setCurrentIndex(index1);	
		//int row=index.row();
		//int col=index.column();
	}

	QStyledItemDelegate::setEditorData(editor,index);

}
void dataWindowDelegate::comboItemChanged(QString text)
{
	parent->getTableData2()->currentItem()->setText(text);
}
void dataWindowDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
	QStyledItemDelegate::setModelData(editor,model,index);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
leftTableRefWindowDelegate::leftTableRefWindowDelegate(QObject *parent)
	: QStyledItemDelegate(parent)
{
	this->parent=static_cast<view_datawindow_edit*>(parent);
}

QWidget* leftTableRefWindowDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	switch(index.column())
	{
	case 2:
		{
			QComboBox* editor = new QComboBox(parent);
			editor->insertItem(0,"固定");
			editor->insertItem(1,"历史库");
			editor->insertItem(2,"内存库");
			return editor;
		}
		break;
	}
	QWidget *widget=QStyledItemDelegate::createEditor(parent,option,index);
	return widget;
}

void leftTableRefWindowDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
	QComboBox* comboEditor = qobject_cast<QComboBox* >(editor);
	if (comboEditor)
	{		
		QString text = index.model()->data(index,Qt::EditRole).toString();
		connect(comboEditor,SIGNAL(currentIndexChanged(QString)),this,SLOT(comboItemChanged(QString)));
		int index1 = comboEditor->findText(text);	

		comboEditor->setCurrentIndex(index1);	
		//int row=index.row();
		//int col=index.column();
	}

	QStyledItemDelegate::setEditorData(editor,index);

}
void leftTableRefWindowDelegate::comboItemChanged(QString text)
{
	parent->getTableRef1()->currentItem()->setText(text);
}
void leftTableRefWindowDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
	QStyledItemDelegate::setModelData(editor,model,index);
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
leftTableDataWindowDelegate::leftTableDataWindowDelegate(QObject *parent)
	: QStyledItemDelegate(parent)
{
	this->parent=static_cast<view_datawindow_edit*>(parent);
}

QWidget* leftTableDataWindowDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	switch(index.column())
	{
	case 2:
		{
			QComboBox* editor = new QComboBox(parent);
			editor->insertItem(0,"历史库");
			editor->insertItem(1,"内存库");
			return editor;
		}
		break;

	case 4:
		{
			QComboBox* editor = new QComboBox(parent);
			editor->insertItem(0,"不变换");
			editor->insertItem(1,"带标题行转置");
			editor->insertItem(3,"数据行转置");
			editor->insertItem(4,"交叉变换");
			return editor;
		}
		break;
	}
	QWidget *widget=QStyledItemDelegate::createEditor(parent,option,index);
	return widget;
}

void leftTableDataWindowDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
	QComboBox* comboEditor = qobject_cast<QComboBox* >(editor);
	if (comboEditor)
	{		
		QString text = index.model()->data(index,Qt::EditRole).toString();
		connect(comboEditor,SIGNAL(currentIndexChanged(QString)),this,SLOT(comboItemChanged(QString)));
		int index1 = comboEditor->findText(text);	
		comboEditor->setCurrentIndex(index1);	
		//int row=index.row();
		//int col=index.column();
	}

	QStyledItemDelegate::setEditorData(editor,index);

}
void leftTableDataWindowDelegate::comboItemChanged(QString text)
{
	parent->getTableData1()->currentItem()->setText(text);
}
void leftTableDataWindowDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
	QStyledItemDelegate::setModelData(editor,model,index);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
OutputCfg::OutputCfg(QWidget *parent)
	: QDialog(parent)
{
	setupUi(this);
	SQt::SetTableWidgetStyle(tableWidget);
	tableWidget->horizontalHeader()->setStretchLastSection(true);
	tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
	tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
	tableWidget->verticalHeader()->hide();
	connect(pushButton,SIGNAL(clicked()),this,SLOT(onPushButtonPressed()));
	connect(pushButton_2,SIGNAL(clicked()),this,SLOT(onPushButton2Pressed()));
	connect(tableWidget->horizontalHeader(),SIGNAL(sectionDoubleClicked(int )),this, SLOT(DoubleClickedTable(int)));
	pDb = DB;
	SString sql = "select cls_name,dw_sn,dstype,name,dbtype,dw_desc,dw_select,transform_type,cross_refname,cross_column,cross_data_column,ext_attr from t_ssp_data_window order by cls_name desc,dw_desc";
	int ret = pDb->RetrieveRecordset(sql,m_rsLeftTable);
	if (ret < 0)
	{
		QMessageBox::warning(this,tr("警告"),tr("在数据库查询失败[%1]").arg(sql.data()),QMessageBox::Ok);
		return;
	}
	sql = "select dw_sn,row_sn,row_id,row_name from t_ssp_data_window_row order by dw_sn,row_sn";
	ret = pDb->RetrieveRecordset(sql,m_rsRow);
	if (ret < 0)
	{
		QMessageBox::warning(this,tr("警告"),tr("在数据库查询失败[%1]").arg(sql.data()),QMessageBox::Ok);
		return;
	}
	sql = "select dw_sn,col_sn,col_name,col_vtype,col_width,ref_name,conv,pkey,isnull,ukey,ext_attr from t_ssp_data_window_col order by dw_sn,col_sn";
	ret = pDb->RetrieveRecordset(sql,m_rsCol);
	if (ret < 0)
	{
		QMessageBox::warning(this,tr("警告"),tr("在数据库查询失败[%1]").arg(sql.data()),QMessageBox::Ok);
		return;
	}
	sql = "select dw_sn,con_sn,con_name,is_option,cmp_type,input_type,ref_name,def_value,dyn_where from t_ssp_data_window_condition order by dw_sn,con_sn";
	ret = pDb->RetrieveRecordset(sql,m_rsCondition);
	if (ret < 0)
	{
		QMessageBox::warning(this,tr("警告"),tr("在数据库查询失败[%1]").arg(sql.data()),QMessageBox::Ok);
		return;
	}
	//表格初始化
	for(int i=0;i<m_rsLeftTable.GetRows();i++)
	{
		tableWidget->insertRow(tableWidget->rowCount());
		QTableWidgetItem *item=new QTableWidgetItem();
		item->setCheckState(Qt::Unchecked);
		item->setText(m_rsLeftTable.GetValue(i,0).data());
		item->setFlags(item->flags() & ~Qt::ItemIsEditable);
		tableWidget->setItem(tableWidget->rowCount()-1,0,item);

		item=new QTableWidgetItem();
		item->setText(m_rsLeftTable.GetValue(i,1).data());
		
		item->setFlags(item->flags() & ~Qt::ItemIsEditable);
		tableWidget->setItem(tableWidget->rowCount()-1,1,item);

		item=new QTableWidgetItem();
		item->setText(tranftype(m_rsLeftTable.GetValue(i,2)).data());
		item->setFlags(item->flags() & ~Qt::ItemIsEditable);
		tableWidget->setItem(tableWidget->rowCount()-1,2,item);

		item=new QTableWidgetItem();
		item->setText(m_rsLeftTable.GetValue(i,3).data());
		item->setFlags(item->flags() & ~Qt::ItemIsEditable);
		tableWidget->setItem(tableWidget->rowCount()-1,3,item);

		item=new QTableWidgetItem();
		item->setText(tranfdbtype(m_rsLeftTable.GetValue(i,4)).data());
		item->setFlags(item->flags() & ~Qt::ItemIsEditable);
		tableWidget->setItem(tableWidget->rowCount()-1,4,item);

		item=new QTableWidgetItem();
		item->setText(m_rsLeftTable.GetValue(i,5).data());
		item->setFlags(item->flags() & ~Qt::ItemIsEditable);
		tableWidget->setItem(tableWidget->rowCount()-1,5,item);

		item=new QTableWidgetItem();
		item->setText(tranftranftype(m_rsLeftTable.GetValue(i,7)).data());
		item->setFlags(item->flags() & ~Qt::ItemIsEditable);
		tableWidget->setItem(tableWidget->rowCount()-1,6,item);

		item=new QTableWidgetItem();
		item->setText(m_rsLeftTable.GetValue(i,8).data());
		item->setFlags(item->flags() & ~Qt::ItemIsEditable);
		tableWidget->setItem(tableWidget->rowCount()-1,7,item);

		item=new QTableWidgetItem();
		item->setText(m_rsLeftTable.GetValue(i,9).data());
		item->setFlags(item->flags() & ~Qt::ItemIsEditable);
		tableWidget->setItem(tableWidget->rowCount()-1,8,item);

		item=new QTableWidgetItem();
		item->setText(m_rsLeftTable.GetValue(i,10).data());
		item->setFlags(item->flags() & ~Qt::ItemIsEditable);
		tableWidget->setItem(tableWidget->rowCount()-1,9,item);

		item=new QTableWidgetItem();
		item->setText(m_rsLeftTable.GetValue(i,11).data());
		item->setFlags(item->flags() & ~Qt::ItemIsEditable);
		tableWidget->setItem(tableWidget->rowCount()-1,10,item);
	}
}

OutputCfg::~OutputCfg()
{

}
void OutputCfg::DoubleClickedTable(int col)
{
	if(col!=0)
		return;
	int rowCnt=tableWidget->rowCount();
	if(tableWidget->item(0,col)->checkState()==Qt::Checked)
		for(int row=0;row<rowCnt;row++) 
			tableWidget->item(row,col)->setCheckState(Qt::Unchecked);
	else
		for(int row=0;row<rowCnt;row++) 
			tableWidget->item(row,col)->setCheckState(Qt::Checked);
}
//输出为xml
void OutputCfg::onPushButtonPressed()
{
	QString path=QFileDialog::getSaveFileName(this,"保存文件","./../conf/sys_datawindow.xml","*.xml",0,0);
	//QString path="D:\\softPlat\\trunk\\SSP01\\project\\win32-vc10\\sys_datawindow.xml";
	if(path=="")
		return;
	SXmlConfig pCfg;
	pCfg.SetNodeName("data_window");
	int ipcount = 0;
	for(int row=0;row<tableWidget->rowCount();row++)
	{
		if(tableWidget->item(row,0)->checkState()==Qt::Checked)
		{
			SXmlConfig *pCfgChild=new SXmlConfig();
			if(tableWidget->item(row,2)->text()==tr("引用"))
			{
				pCfgChild->SetNodeName("reference");
				pCfgChild->SetAttribute("name",tableWidget->item(row,3)->text().toStdString().data());
				pCfgChild->SetAttribute("dbtype",tranfdbtype(tableWidget->item(row,4)->text().toStdString().data()));
				pCfgChild->SetAttribute("dw_desc",tableWidget->item(row,5)->text().toStdString().data());
				pCfgChild->SetAttribute("cls_name",tableWidget->item(row,0)->text().toStdString().data());
				if(tableWidget->item(row,4)->text()=="固定")
				{
					for(int j=0;j<m_rsRow.GetRows();j++)
					{
						if(m_rsRow.GetValue(j,0)==tableWidget->item(row,1)->text().toStdString().data())
						{
							SBaseConfig *rowNode=pCfgChild->AddChildNode("row");
							rowNode->SetAttribute("row_name",m_rsRow.GetValue(j,3));
							rowNode->SetAttribute("row_id",m_rsRow.GetValue(j,2));
						}
					}
				}
				else if(tableWidget->item(row,4)->text()=="历史库"||tableWidget->item(row,4)->text()=="内存库")
				{
					SBaseConfig *rowNode=pCfgChild->AddChildNode("select");
					int i=0;
					for(int j=0;j<m_rsLeftTable.GetRows();j++)
					{
						if(m_rsLeftTable.GetValue(j,1)==tableWidget->item(row,1)->text().toStdString().data())
						{
							i=j;
							break;
						}
					}
					SString sql=m_rsLeftTable.GetValue(i,6);
					sql.replace("<","&lt;");//转义
					sql.replace(">","&gt;");//转义
					//sql.replace("\'","&apos;");//转义
					rowNode->SetNodeValue(sql);
				}
				
			}
			if(tableWidget->item(row,2)->text()=="数据集")
			{
				pCfgChild->SetNodeName("dataset");
				pCfgChild->SetAttribute("name",tableWidget->item(row,3)->text().toStdString().data());
				pCfgChild->SetAttribute("dbtype",tranfdbtype(tableWidget->item(row,4)->text().toStdString().data()));
				pCfgChild->SetAttribute("dw_desc",tableWidget->item(row,5)->text().toStdString().data());
				pCfgChild->SetAttribute("transform_type",tranftranftype(tableWidget->item(row,6)->text().toStdString().data()));
				pCfgChild->SetAttribute("cross_refname",tableWidget->item(row,7)->text().toStdString().data());
				pCfgChild->SetAttribute("cross_column",tableWidget->item(row,8)->text().toStdString().data());
				pCfgChild->SetAttribute("cross_data_column",tableWidget->item(row,9)->text().toStdString().data());
				pCfgChild->SetAttribute("cls_name",tableWidget->item(row,0)->text().toStdString().data());
				pCfgChild->SetAttribute("ext_attr",tableWidget->item(row,10)->text().toStdString().data());
				SBaseConfig *rowNode=pCfgChild->AddChildNode("select");
				int i=0;
				for(int j=0;j<m_rsLeftTable.GetRows();j++)
				{
					if(m_rsLeftTable.GetValue(j,1)==tableWidget->item(row,1)->text().toStdString().data())
					{
						i=j;
						break;
					}
				}
				SString sql=m_rsLeftTable.GetValue(i,6);
				sql.replace("<","&lt;");//转义
				sql.replace(">","&gt;");//转义
				//sql.replace("\'","&apos;");//转义
				rowNode->SetNodeValue(sql);
				bool flag=false;
				//插入列定义行信息
				SBaseConfig *rowNodeR=NULL;
				for(int j=0;j<m_rsCol.GetRows();j++)
				{
					if(m_rsCol.GetValue(j,0)==tableWidget->item(row,1)->text().toStdString().data())
					{
						if(flag==false)
						{
							rowNodeR=pCfgChild->AddChildNode("columns");
							flag=true;
						}
						SBaseConfig *rowNode=rowNodeR->AddChildNode("col");
						rowNode->SetAttribute("col_name",m_rsCol.GetValue(j,2));
						rowNode->SetAttribute("col_vtype",m_rsCol.GetValue(j,3));
						rowNode->SetAttribute("col_width",m_rsCol.GetValue(j,4));
						rowNode->SetAttribute("ref_name",m_rsCol.GetValue(j,5));
						rowNode->SetAttribute("conv",m_rsCol.GetValue(j,6));
						rowNode->SetAttribute("pkey",m_rsCol.GetValue(j,7));
						rowNode->SetAttribute("isnull",m_rsCol.GetValue(j,8));
						rowNode->SetAttribute("ukey",m_rsCol.GetValue(j,9));
						rowNode->SetAttribute("ext_attr",m_rsCol.GetValue(j,10));
					}
				}
				//插入条件定义行信息
				SBaseConfig *rowNodeC=NULL;
				flag = false;
				for(int j=0;j<m_rsCondition.GetRows();j++)
				{
					if(m_rsCondition.GetValue(j,0)==tableWidget->item(row,1)->text().toStdString().data())
					{
						if(flag==false)
						{
							rowNodeC=pCfgChild->AddChildNode("condition");
							flag=true;
						}
						SBaseConfig *rowNode=rowNodeC->AddChildNode("con");
						rowNode->SetAttribute("con_name",m_rsCondition.GetValue(j,2));
						rowNode->SetAttribute("is_option",m_rsCondition.GetValue(j,3));
						rowNode->SetAttribute("cmp_type",m_rsCondition.GetValue(j,4));
						rowNode->SetAttribute("input_type",m_rsCondition.GetValue(j,5));
						rowNode->SetAttribute("ref_name",m_rsCondition.GetValue(j,6));
						rowNode->SetAttribute("def_value",m_rsCondition.GetValue(j,7));
						rowNode->SetAttribute("dyn_where",m_rsCondition.GetValue(j,8));
					}
				}
			}
			ipcount++;
			pCfg.AddChildNode(pCfgChild);
		}

	}

	if(pCfg.SaveConfig(path.toStdString().data()))
		QMessageBox::information(this,tr("提示"),tr("导出成功"),QMessageBox::Ok);
	else
		QMessageBox::information(this,tr("提示"),tr("导出失败"),QMessageBox::Ok);
	close();
}
void OutputCfg::onPushButton2Pressed()
{
	close();
}

void OutputCfg::setupUi(QWidget *OutputCfg)
{
	if (OutputCfg->objectName().isEmpty())
		OutputCfg->setObjectName(QString::fromUtf8("OutputCfg"));
	OutputCfg->resize(451, 375);
	gridLayout_2 = new QGridLayout(OutputCfg);
	gridLayout_2->setSpacing(5);
	gridLayout_2->setContentsMargins(4, 4, 4, 4);
	gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
	verticalLayout = new QVBoxLayout();
	verticalLayout->setSpacing(6);
	verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
	tableWidget = new QTableWidget(OutputCfg);
	if (tableWidget->columnCount() < 11)
		tableWidget->setColumnCount(11);
	QTableWidgetItem *__qtablewidgetitem = new QTableWidgetItem();
	tableWidget->setHorizontalHeaderItem(0, __qtablewidgetitem);
	QTableWidgetItem *__qtablewidgetitem1 = new QTableWidgetItem();
	tableWidget->setHorizontalHeaderItem(1, __qtablewidgetitem1);
	QTableWidgetItem *__qtablewidgetitem2 = new QTableWidgetItem();
	tableWidget->setHorizontalHeaderItem(2, __qtablewidgetitem2);
	QTableWidgetItem *__qtablewidgetitem3 = new QTableWidgetItem();
	tableWidget->setHorizontalHeaderItem(3, __qtablewidgetitem3);
	QTableWidgetItem *__qtablewidgetitem4 = new QTableWidgetItem();
	tableWidget->setHorizontalHeaderItem(4, __qtablewidgetitem4);
	QTableWidgetItem *__qtablewidgetitem5 = new QTableWidgetItem();
	tableWidget->setHorizontalHeaderItem(5, __qtablewidgetitem5);
	QTableWidgetItem *__qtablewidgetitem6 = new QTableWidgetItem();
	tableWidget->setHorizontalHeaderItem(6, __qtablewidgetitem6);
	QTableWidgetItem *__qtablewidgetitem7 = new QTableWidgetItem();
	tableWidget->setHorizontalHeaderItem(7, __qtablewidgetitem7);
	QTableWidgetItem *__qtablewidgetitem8 = new QTableWidgetItem();
	tableWidget->setHorizontalHeaderItem(8, __qtablewidgetitem8);
	QTableWidgetItem *__qtablewidgetitem9 = new QTableWidgetItem();
	tableWidget->setHorizontalHeaderItem(9, __qtablewidgetitem9);
	QTableWidgetItem *__qtablewidgetitem10 = new QTableWidgetItem();
	tableWidget->setHorizontalHeaderItem(10, __qtablewidgetitem10);
	tableWidget->setObjectName(QString::fromUtf8("tableWidget"));
	verticalLayout->addWidget(tableWidget);
	widget = new QWidget(OutputCfg);
	widget->setObjectName(QString::fromUtf8("widget"));
	widget->setMaximumSize(QSize(16777215, 30));
	gridLayout = new QGridLayout(widget);
	gridLayout->setSpacing(4);
	gridLayout->setContentsMargins(0, 0, 0, 0);
	gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
	pushButton = new QPushButton(widget);
	pushButton->setObjectName(QString::fromUtf8("pushButton"));
	QIcon icon;
	icon.addFile(QString::fromUtf8(":/ssp_gui/Resources/export2.gif"), QSize(), QIcon::Normal, QIcon::Off);
	pushButton->setIcon(icon);
	gridLayout->addWidget(pushButton, 0, 0, 1, 1);
	pushButton_2 = new QPushButton(widget);
	pushButton_2->setObjectName(QString::fromUtf8("pushButton_2"));
	QIcon icon1;
	icon1.addFile(QString::fromUtf8(":/ssp_gui/Resources/del.gif"), QSize(), QIcon::Normal, QIcon::Off);
	pushButton_2->setIcon(icon1);
	gridLayout->addWidget(pushButton_2, 0, 1, 1, 1);
	horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
	gridLayout->addItem(horizontalSpacer, 0, 2, 1, 1);
	verticalLayout->addWidget(widget);
	gridLayout_2->addLayout(verticalLayout, 0, 0, 1, 1);
	retranslateUi(OutputCfg);
	QMetaObject::connectSlotsByName(OutputCfg);
	//去掉对话框上“？”按钮
	Qt::WindowFlags flags=Qt::Dialog;
	flags |=Qt::WindowCloseButtonHint;
	setWindowFlags(flags);
} // setupUi

void OutputCfg::retranslateUi(QWidget *OutputCfg)
{
	OutputCfg->setWindowTitle(tr("选择输出"));
	QTableWidgetItem *___qtablewidgetitem0 = tableWidget->horizontalHeaderItem(0);
	___qtablewidgetitem0->setText( tr("分类"));
	QTableWidgetItem *___qtablewidgetitem = tableWidget->horizontalHeaderItem(1);
	___qtablewidgetitem->setText( tr("序号"));
	QTableWidgetItem *___qtablewidgetitem1 = tableWidget->horizontalHeaderItem(2);
	___qtablewidgetitem1->setText( tr("类型"));
	QTableWidgetItem *___qtablewidgetitem2 = tableWidget->horizontalHeaderItem(3);
	___qtablewidgetitem2->setText(tr("名称"));
	QTableWidgetItem *___qtablewidgetitem3 = tableWidget->horizontalHeaderItem(4);
	___qtablewidgetitem3->setText(tr("数据类型"));
	QTableWidgetItem *___qtablewidgetitem4 = tableWidget->horizontalHeaderItem(5);
	___qtablewidgetitem4->setText(tr("描述"));
	QTableWidgetItem *___qtablewidgetitem5 = tableWidget->horizontalHeaderItem(6);
	___qtablewidgetitem5->setText( tr("数据变换类型"));
	QTableWidgetItem *___qtablewidgetitem6 = tableWidget->horizontalHeaderItem(7);
	___qtablewidgetitem6->setText(tr("交叉数据集引用名"));
	QTableWidgetItem *___qtablewidgetitem7 = tableWidget->horizontalHeaderItem(8);
	___qtablewidgetitem7->setText(tr("交叉列序号"));
	QTableWidgetItem *___qtablewidgetitem8 = tableWidget->horizontalHeaderItem(9);
	___qtablewidgetitem8->setText(tr("交叉数据列序号"));
	QTableWidgetItem *___qtablewidgetitem9 = tableWidget->horizontalHeaderItem(10);
	___qtablewidgetitem9->setText(tr("扩展属性"));
	pushButton->setText(tr("导出(&O)"));
	pushButton_2->setText(tr("取消(&C)"));
} // retranslateUi
SString OutputCfg::tranftranftype(SString str)
{
	if(str=="0")
		return "不变换";
	if(str=="1")
		return "带标题行转置";
	if(str=="2")
		return "数据行转置";
	if(str=="3")
		return "交叉变换";
	if(str=="不变换")
		return "0";
	if(str=="带标题行转置")
		return "1";
	if(str=="数据行转置")
		return "2";
	if(str=="交叉变换")
		return "3";
	return "";
}
SString OutputCfg::tranfdbtype(SString str)
{
	if(str=="1")
		return "固定";
	if(str=="2")
		return "历史库";
	if(str=="3")
		return "内存库";
	if(str=="固定")
		return "fixed";
	if(str=="历史库")
		return "db";
	if(str=="内存库")
		return "mdb";
	return "";
}
SString OutputCfg::tranftype(SString str)
{
	if(str=="1")
		return "引用";
	if(str=="2")
		return "数据集";
	if(str=="引用")
		return "1";
	if(str=="数据集")
		return "2";
	return "";
}

//////////////////////////////////////////////////
inputNum::inputNum(QWidget *parent)
	: QDialog(parent)
{
	setupUi(this);
	connect(pushButton,SIGNAL(clicked()),this,SLOT(onPushButtonPressed()));
	connect(pushButton_2,SIGNAL(clicked()),this,SLOT(onPushButton2Pressed()));
}

inputNum::~inputNum()
{

}
void inputNum::onPushButtonPressed()
{
	emit finishInputNum();
	hide();
}
void inputNum::onPushButton2Pressed()
{
	hide();
}
int inputNum::getNum()
{
	if(lineEdit->text().toInt()>=1)
		return lineEdit->text().toInt();
	else
		return 1;
}
void inputNum::setupUi(QDialog *inputNum)
{
	if (inputNum->objectName().isEmpty())
		inputNum->setObjectName(QString::fromUtf8("inputNum"));
	inputNum->setEnabled(true);
	inputNum->resize(250, 90);
	QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	sizePolicy.setHorizontalStretch(0);
	sizePolicy.setVerticalStretch(0);
	sizePolicy.setHeightForWidth(inputNum->sizePolicy().hasHeightForWidth());
	inputNum->setSizePolicy(sizePolicy);
	inputNum->setMinimumSize(QSize(250, 90));
	inputNum->setMaximumSize(QSize(250, 90));
	gridLayout = new QGridLayout(inputNum);
	gridLayout->setSpacing(4);
	gridLayout->setContentsMargins(4, 4, 4, 4);
	gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
	verticalLayout = new QVBoxLayout();
	verticalLayout->setSpacing(6);
	verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
	horizontalLayout_2 = new QHBoxLayout();
	horizontalLayout_2->setSpacing(6);
	horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
	label = new QLabel(inputNum);
	label->setObjectName(QString::fromUtf8("label"));
	horizontalLayout_2->addWidget(label);
	lineEdit = new QLineEdit(inputNum);
	lineEdit->setObjectName(QString::fromUtf8("lineEdit"));
	horizontalLayout_2->addWidget(lineEdit);
	verticalLayout->addLayout(horizontalLayout_2);
	widget = new QWidget(inputNum);
	widget->setObjectName(QString::fromUtf8("widget"));
	widget->setMaximumSize(QSize(16777215, 30));
	horizontalLayout = new QHBoxLayout(widget);
	horizontalLayout->setSpacing(4);
	horizontalLayout->setContentsMargins(11, 11, 11, 11);
	horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
	horizontalLayout->setContentsMargins(0, 0, 0, 4);
	horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
	horizontalLayout->addItem(horizontalSpacer);
	pushButton = new QPushButton(widget);
	pushButton->setObjectName(QString::fromUtf8("pushButton"));
	QIcon icon;
	icon.addFile(QString::fromUtf8(":/ssp_gui/Resources/export2.gif"), QSize(), QIcon::Normal, QIcon::Off);
	pushButton->setIcon(icon);
	horizontalLayout->addWidget(pushButton);
	pushButton_2 = new QPushButton(widget);
	pushButton_2->setObjectName(QString::fromUtf8("pushButton_2"));
	QIcon icon1;
	icon1.addFile(QString::fromUtf8(":/ssp_gui/Resources/del.gif"), QSize(), QIcon::Normal, QIcon::Off);
	pushButton_2->setIcon(icon1);
	horizontalLayout->addWidget(pushButton_2);
	verticalLayout->addWidget(widget);
	gridLayout->addLayout(verticalLayout, 0, 0, 1, 1);
	retranslateUi(inputNum);
	QMetaObject::connectSlotsByName(inputNum);
	Qt::WindowFlags flags=Qt::Dialog;
	flags |=Qt::WindowCloseButtonHint;
	setWindowFlags(flags);
} // setupUi

void inputNum::retranslateUi(QDialog *inputNum)
{
	inputNum->setWindowTitle(tr("输入窗口"));
	label->setText(tr("输入复制个数"));
	lineEdit->setText(tr("1"));
	pushButton->setText(tr("确定(&O)"));
	pushButton_2->setText(tr("取消(&C)"));
} // retranslateUi
///////////////////////////////////////////////

review::review(QWidget *parent)
	: QDialog(parent)
{
	setupUi(this);
	setWindowFlags( this->windowFlags() | Qt::WindowMaximizeButtonHint | Qt::WindowMinimizeButtonHint );
	showMaximized();
}

review::~review()
{

}
void review::setupUi(QDialog *review)
{
	if (review->objectName().isEmpty())
		review->setObjectName(QString::fromUtf8("review"));
	review->resize(594, 342);
	gridLayout = new QGridLayout(review);
	gridLayout->setSpacing(6);
	gridLayout->setContentsMargins(4, 4, 4, 4);
	gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
	tabWidget = new QTabWidget(review);
	tabWidget->setObjectName(QString::fromUtf8("tabWidget"));
	widget = new QWidget();
	widget->setObjectName(QString::fromUtf8("widget"));
	gridLayout_3 = new QGridLayout(widget);
	gridLayout_3->setSpacing(4);
	gridLayout_3->setContentsMargins(4, 4, 4, 4);
	gridLayout_3->setObjectName(QString::fromUtf8("gridLayout_3"));
	gridLayout_2 = new QGridLayout();
	gridLayout_2->setSpacing(4);
	gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
	gridLayout_3->addLayout(gridLayout_2, 0, 0, 1, 1);
	tabWidget->addTab(widget, QString());
	widget_2 = new QWidget();
	widget_2->setObjectName(QString::fromUtf8("widget_2"));
	gridLayout_5 = new QGridLayout(widget_2);
	gridLayout_5->setSpacing(4);
	gridLayout_5->setContentsMargins(4, 4, 4, 4);
	gridLayout_5->setObjectName(QString::fromUtf8("gridLayout_5"));
	gridLayout_4 = new QGridLayout();
	gridLayout_4->setSpacing(4);
	gridLayout_4->setObjectName(QString::fromUtf8("gridLayout_4"));
	gridLayout_5->addLayout(gridLayout_4, 0, 0, 1, 1);
	tabWidget->addTab(widget_2, QString());
	gridLayout->addWidget(tabWidget, 0, 0, 1, 1);
	retranslateUi(review);
	tabWidget->setCurrentIndex(1);
	QMetaObject::connectSlotsByName(review);
} // setupUi

void review::retranslateUi(QDialog *review)
{
	review->setWindowTitle(tr("预览窗口"));
	tabWidget->setTabText(tabWidget->indexOf(widget), tr("编辑窗口"));
	tabWidget->setTabText(tabWidget->indexOf(widget_2), tr("查询窗口"));
} // retranslateUi


//设置引用页的table和qtexteditor控件是否阻塞信号
void view_datawindow_edit::setRefBlockSignals(bool blcok)
{
	ui.tableWidget_ref1->blockSignals(blcok);
	ui.tableWidget_ref2->blockSignals(blcok);
	ui.textEdit_ref->blockSignals(blcok);
}
//设置数据集页的table和qtexteditor控件是否阻塞信号
void view_datawindow_edit::setDataBlockSignals(bool blcok)
{
	ui.tableWidget_data1->blockSignals(blcok);
	ui.tableWidget_data2->blockSignals(blcok);
	ui.textEdit_data->blockSignals(blcok);
}