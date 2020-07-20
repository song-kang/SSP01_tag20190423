#include "view_svglib_edit.h"
#include "view_uicfg.h"
#include "gui/qt/SSvgEditorWnd.h"
#include "extattributedlg.h"
//20151024 svg数据从表格改为从数据库读取，表格布局调整。导出界面改为树形
//20151026 布局调整，删除问题修正，左侧表格键盘响应，新建给出默认名，导出默认全选,导出删除多余空行,BLOB读取修改，添加应用按钮和将数据库svg数据直接写入默认类型静态函数
view_svglib_edit::view_svglib_edit(QWidget *parent)
	: CBaseView(parent)
{
	ui.setupUi(this);
	m_inputDlg=new inputNum1(this);
	connect(ui.pushButton_add,SIGNAL(clicked()),this,SLOT(onPushButton_add()));
	connect(ui.pushButton_edit,SIGNAL(clicked()),this,SLOT(onPushButton_edit()));
	connect(ui.pushButton_del,SIGNAL(clicked()),this,SLOT(onPushButton_del()));

	connect(ui.pushButton_up,SIGNAL(clicked()),this,SLOT(onPushButton_up()));
	connect(ui.pushButton_down,SIGNAL(clicked()),this,SLOT(onPushButton_down()));
	connect(ui.pushButton_copy,SIGNAL(clicked()),this,SLOT(onPushButton_copy()));

	connect(ui.pushButton_add_svg,SIGNAL(clicked()),this,SLOT(onPushButton_add_svg()));
	connect(ui.pushButton_edit_svg,SIGNAL(clicked()),this,SLOT(onPushButton_edit_svg()));
	connect(ui.pushButton_del_svg,SIGNAL(clicked()),this,SLOT(onPushButton_del_svg()));

	connect(ui.pushButton_copy_svg,SIGNAL(clicked()),this,SLOT(onPushButton_copy_svg()));
	connect(ui.pushButton_up_svg,SIGNAL(clicked()),this,SLOT(onPushButton_up_svg()));
	connect(ui.pushButton_down_svg,SIGNAL(clicked()),this,SLOT(onPushButton_down_svg()));
	connect(ui.pushButton_othtype_svg,SIGNAL(clicked()),this,SLOT(onPushButton_othtype_svg()));

	connect(ui.pushButton_import,SIGNAL(clicked()),this,SLOT(onPushButton_import()));
	connect(ui.pushButton_export,SIGNAL(clicked()),this,SLOT(onPushButton_export()));
	connect(ui.pushButton_app,SIGNAL(clicked()),this,SLOT(onPushButton_app()));
	ui.pushButton_edit->hide();
	connect(ui.tableWidget_type,SIGNAL(currentItemChanged(QTableWidgetItem *,QTableWidgetItem *)),this,SLOT(showSvgInfo(QTableWidgetItem *,QTableWidgetItem *)));
	pDb = DB;m_svgEditWnd=NULL;
// 	if(pDb->RetrieveRecordset("select svgtype_sn,svgtype_name from t_ssp_svglib_type order by svgtype_sn",m_typeTable)<=0)
// 	{
// 		QMessageBox::warning(this,tr("警告"),tr("数据表为空"),QMessageBox::Ok);
// 	}
	QStringList header;
	header<<tr("序号")<<tr("名称");
	ui.tableWidget_type->setColumnCount(2);
	ui.tableWidget_type->setHorizontalHeaderLabels(header);
	header.clear();
	header<<tr("序号")<<tr("类型")<<tr("名称");//<<tr("图元内容");
	ui.tableWidget_svg->setColumnCount(3);
	ui.tableWidget_svg->setHorizontalHeaderLabels(header);
	ui.tableWidget_svg->hideColumn(1);
	header.clear();
	//ui.tableWidget_svg->setColumnHidden(3,true);

	ui.tableWidget_type->setSelectionBehavior(QAbstractItemView::SelectRows);
	ui.tableWidget_type->setSelectionMode(QAbstractItemView::SingleSelection);
	ui.tableWidget_type->horizontalHeader()->setResizeMode(QHeaderView::ResizeToContents);
	ui.tableWidget_type->verticalHeader()->hide();
	ui.tableWidget_svg->setSelectionBehavior(QAbstractItemView::SelectRows);
	//ui.tableWidget_svg->setSelectionMode(QAbstractItemView::SingleSelection);
	ui.tableWidget_svg->horizontalHeader()->setResizeMode(QHeaderView::ResizeToContents);
	//ui.tableWidget_svg->verticalHeader()->hide();
	SQt::SetTableWidgetStyle(ui.tableWidget_type);
	SQt::SetTableWidgetStyle(ui.tableWidget_svg);

	QList<int> widgetSizes;//splitter间距调整
	widgetSizes << 300 << 700;
	ui.splitter->setSizes(widgetSizes);

	connect(ui.tableWidget_type,SIGNAL(itemChanged(QTableWidgetItem *)),this,SLOT(editType(QTableWidgetItem *)));
	connect(ui.tableWidget_svg,SIGNAL(itemChanged(QTableWidgetItem *)),this,SLOT(editSvg(QTableWidgetItem *)));
	refreshTypeTable();
}

view_svglib_edit::~view_svglib_edit()
{

}

void view_svglib_edit::refreshTypeTable()
{
	ui.tableWidget_svg->blockSignals(true);
	ui.tableWidget_type->blockSignals(true);
	//刷新类型表格首先初始化集表格
	ui.tableWidget_svg->clear();
	while(ui.tableWidget_svg->rowCount()!=0)
		ui.tableWidget_svg->removeRow(0);
	QStringList header;
	header<<tr("序号")<<tr("类型")<<tr("名称");//<<tr("图元内容");
	ui.tableWidget_svg->setColumnCount(3);
	ui.tableWidget_svg->setHorizontalHeaderLabels(header);
	header.clear();
	m_typeTable.clear();

 	if(pDb->RetrieveRecordset("select svgtype_sn,svgtype_name from t_ssp_svglib_type order by svgtype_sn",m_typeTable)<=0)
 	{
// 		QMessageBox::warning(this,tr("警告"),tr("数据表为空"),QMessageBox::Ok);
 	}

	for(int i=0;i<m_typeTable.GetRows();i++)
	{	
		ui.tableWidget_type->insertRow(ui.tableWidget_type->rowCount());
		QTableWidgetItem *item=new QTableWidgetItem(m_typeTable.GetValue(i,0).data());
		item->setFlags(item->flags() & ~Qt::ItemIsEditable);
		ui.tableWidget_type->setItem(ui.tableWidget_type->rowCount()-1,0,item);

		item=new QTableWidgetItem(m_typeTable.GetValue(i,1).data());
		ui.tableWidget_type->setItem(ui.tableWidget_type->rowCount()-1,1,item);
	}
	ui.tableWidget_svg->blockSignals(false);
	ui.tableWidget_type->blockSignals(false);
}

void view_svglib_edit::showSvgInfo(QTableWidgetItem *item,QTableWidgetItem *pre)
{
	Q_UNUSED(pre);
	ui.tableWidget_svg->blockSignals(true);
	ui.tableWidget_type->blockSignals(true);
	//刷新类型表格首先初始化集表格

	ui.tableWidget_svg->clear();
	while(ui.tableWidget_svg->rowCount()!=0)
		ui.tableWidget_svg->removeRow(0);
	QStringList header;
	header<<tr("序号")<<tr("类型")<<tr("名称");//<<tr("图元内容");
	ui.tableWidget_svg->setColumnCount(3);
	ui.tableWidget_svg->setHorizontalHeaderLabels(header);
	header.clear();
	m_svgTable.clear();

	int row=item->row();
	int curNo=ui.tableWidget_type->item(row,0)->text().toInt();
	QString groupName="图元集("+ui.tableWidget_type->item(row,1)->text()+")";
	ui.groupBox_2->setTitle(groupName);

	int flag=pDb->RetrieveRecordset(SString::toFormat("select svg_sn,svgtype_sn,svg_name,svg_file from t_ssp_svglib_item where svgtype_sn=%d order by svg_sn",curNo),m_svgTable);

	if(flag<0)
	{
		QMessageBox::warning(this,tr("警告"),tr("数据库读取图元出错"),QMessageBox::Ok);
		ui.tableWidget_svg->blockSignals(false);
		ui.tableWidget_type->blockSignals(false);
		return;
	}
	for(int i=0;i<m_svgTable.GetRows();i++)
	{
		ui.tableWidget_svg->insertRow(ui.tableWidget_svg->rowCount());
		QTableWidgetItem *item=new QTableWidgetItem(m_svgTable.GetValue(i,0).data());
		item->setFlags(item->flags() & ~Qt::ItemIsEditable);
		ui.tableWidget_svg->setItem(ui.tableWidget_svg->rowCount()-1,0,item);

		item=new QTableWidgetItem(m_svgTable.GetValue(i,1).data());
		item->setFlags(item->flags() & ~Qt::ItemIsEditable);
		ui.tableWidget_svg->setItem(ui.tableWidget_svg->rowCount()-1,1,item);

		item=new QTableWidgetItem(m_svgTable.GetValue(i,2).data());
		ui.tableWidget_svg->setItem(ui.tableWidget_svg->rowCount()-1,2,item);
// 		item=new QTableWidgetItem(m_svgTable.GetValue(i,3).data());
// 		item->setFlags(item->flags() & ~Qt::ItemIsEditable);
// 		ui.tableWidget_svg->setItem(ui.tableWidget_svg->rowCount()-1,3,item);
	}
	ui.tableWidget_svg->blockSignals(false);
	ui.tableWidget_type->blockSignals(false);
}
void view_svglib_edit::onPushButton_add()
{
	ui.tableWidget_type->blockSignals(true);
	int curRow;
	curRow=ui.tableWidget_type->currentRow();
	if(curRow==-1)
		curRow=ui.tableWidget_type->rowCount();
	
	bool flag1=true;
	int num=1;
	while(flag1)
	{
		flag1=false;
		for(int row=0;row<ui.tableWidget_type->rowCount();row++)
		{
			if(ui.tableWidget_type->item(row,0)->text().toInt()==num)
			{
				num++;
				flag1=true;
			}
		}
	}
	ui.tableWidget_type->insertRow(curRow);
	QTableWidgetItem *item=new QTableWidgetItem();
	item->setText(QString::number(num));
	ui.tableWidget_type->setItem(curRow,0,item);

	item=new QTableWidgetItem("新类型");
	ui.tableWidget_type->setItem(curRow,1,item);
	bool flag=pDb->Execute(SString::toFormat("insert into t_ssp_svglib_type (svgtype_sn,svgtype_name) \
											 values (%s,'新类型')",QString::number(num).toStdString().c_str()));
	if(flag==false)
		QMessageBox::warning(this,tr("警告"),tr("数据库插入图元类型失败"),QMessageBox::Ok);
	ui.tableWidget_type->blockSignals(false);
}
void view_svglib_edit::onPushButton_edit()
{
	//
}

void view_svglib_edit::onPushButton_copy()
{
	ui.tableWidget_type->blockSignals(true);
	int curRow=ui.tableWidget_type->currentRow();
	if(curRow<0)
	{
		ui.tableWidget_type->blockSignals(false);
		return;
	}
	bool ok;  
	int cpyNum= QInputDialog::getInteger(this,tr("复制个数"),  
		tr("输入复制个数:"), 1,1,150,1,&ok); 
	if(!ok)
		return;
	if(cpyNum<1)
	{
		ui.tableWidget_type->blockSignals(false);
		return;
	}
	int maxTypesn=pDb->SelectIntoI("select max(svgtype_sn) from t_ssp_svglib_type");
	int maxSvgsn=pDb->SelectIntoI("select max(svg_sn) from t_ssp_svglib_item");
	maxTypesn++;maxSvgsn++;
	

	for(int i=0;i<cpyNum;i++)
	{
		bool flag=pDb->Execute(SString::toFormat("insert into t_ssp_svglib_type(svgtype_sn,svgtype_name) \
												 values (%d,(select svgtype_name from t_ssp_svglib_type as x where svgtype_sn=%s))",\
												 maxTypesn,ui.tableWidget_type->item(curRow,0)->text().toStdString().c_str()));

		if(flag==false)
		{
			QMessageBox::warning(this,tr("警告"),tr("在数据库复制类型失败"),QMessageBox::Ok);
			ui.tableWidget_type->blockSignals(false);
			return;
		}
		SRecordset m_rsRow;
		m_rsRow.clear();
		pDb->RetrieveRecordset("select svg_sn,svgtype_sn,svg_name,svg_file from t_ssp_svglib_item order by svg_sn",m_rsRow);
		//对应复制
		for(int row=0;row<m_rsRow.GetRows();row++)
		{
			if(m_rsRow.GetValue(row,1)==ui.tableWidget_type->item(curRow,0)->text().toStdString().data())
			{
				flag=pDb->Execute(SString::toFormat
					("insert into t_ssp_svglib_item (svg_sn,svgtype_sn,svg_name,svg_file) \
					 values (%d,%d,(select svg_name from t_ssp_svglib_item as x where svg_sn=%s),\
					 (select svg_file from t_ssp_svglib_item as x where svg_sn=%s))",\
					 maxSvgsn,maxTypesn,m_rsRow.GetValue(row,0).data(),m_rsRow.GetValue(row,0).data()));

			}
			maxSvgsn++;
		}
		if(flag==false)
		{
			QMessageBox::warning(this,tr("警告"),tr("在数据库复制引用对应行失败"),QMessageBox::Ok);
			ui.tableWidget_type->blockSignals(false);
			return;
		}
		ui.tableWidget_type->insertRow(ui.tableWidget_type->rowCount());
		QTableWidgetItem *item=new QTableWidgetItem();
		item->setText(QString::number(maxTypesn));
		ui.tableWidget_type->setItem(ui.tableWidget_type->rowCount()-1,0,item);

		item=new QTableWidgetItem();
		item->setText(ui.tableWidget_type->item(curRow,1)->text());
		ui.tableWidget_type->setItem(ui.tableWidget_type->rowCount()-1,1,item);
		maxTypesn++;
	}
	ui.tableWidget_type->blockSignals(false);
}
void view_svglib_edit::onPushButton_up()
{
	ui.tableWidget_type->blockSignals(true);
	int curRow;
	curRow=ui.tableWidget_type->currentRow();
	if(curRow==-1||curRow==0)
	{
		ui.tableWidget_type->blockSignals(false);
		return;
	}
	int sn1=ui.tableWidget_type->item(curRow-1,0)->text().toInt();
	int sn2=ui.tableWidget_type->item(curRow,0)->text().toInt();
	QString text1=ui.tableWidget_type->item(curRow-1,1)->text();
	QString text2=ui.tableWidget_type->item(curRow,1)->text();
	bool flag=pDb->Execute(SString::toFormat("update t_ssp_svglib_type set svgtype_sn=case when svgtype_sn=%d then %d when svgtype_sn=%d then %d else svgtype_sn end where svgtype_sn in (%d,%d)",sn1,sn2+10000,sn2,sn1,sn1,sn2));
	flag=pDb->Execute(SString::toFormat("update t_ssp_svglib_type set svgtype_sn=%d where svgtype_sn=%d",sn2,sn2+10000));
	flag=pDb->Execute(SString::toFormat("update t_ssp_svglib_item set svgtype_sn=%d where svgtype_sn=%d",sn1+10000,sn2));
	flag=pDb->Execute(SString::toFormat("update t_ssp_svglib_item set svgtype_sn=%d where svgtype_sn=%d",sn2,sn1));
	flag=pDb->Execute(SString::toFormat("update t_ssp_svglib_item set svgtype_sn=%d where svgtype_sn=%d",sn1,sn1+10000));
	if(flag==false)
	{
		ui.tableWidget_type->blockSignals(false);
		return;
	}
	ui.tableWidget_type->item(curRow-1,1)->setText(text2);
	ui.tableWidget_type->item(curRow,1)->setText(text1);
	ui.tableWidget_type->setCurrentItem(ui.tableWidget_type->item(curRow-1,1));
	ui.tableWidget_type->blockSignals(false);
}
void view_svglib_edit::onPushButton_down()
{
	ui.tableWidget_type->blockSignals(true);
	int curRow;
	curRow=ui.tableWidget_type->currentRow();
	if(curRow==-1||curRow==ui.tableWidget_type->rowCount()-1)
	{
		ui.tableWidget_type->blockSignals(false);
		return;
	}
	int sn1=ui.tableWidget_type->item(curRow,0)->text().toInt();
	int sn2=ui.tableWidget_type->item(curRow+1,0)->text().toInt();
	QString text1=ui.tableWidget_type->item(curRow,1)->text();
	QString text2=ui.tableWidget_type->item(curRow+1,1)->text();
	bool flag=pDb->Execute(SString::toFormat("update t_ssp_svglib_type set svgtype_sn=case when svgtype_sn=%d then %d when svgtype_sn=%d then %d else svgtype_sn end where svgtype_sn in (%d,%d)",sn1,sn2+10000,sn2,sn1,sn1,sn2));
	flag=pDb->Execute(SString::toFormat("update t_ssp_svglib_type set svgtype_sn=%d where svgtype_sn=%d",sn2,sn2+10000));
	flag=pDb->Execute(SString::toFormat("update t_ssp_svglib_item set svgtype_sn=%d where svgtype_sn=%d",sn1+10000,sn2));
	flag=pDb->Execute(SString::toFormat("update t_ssp_svglib_item set svgtype_sn=%d where svgtype_sn=%d",sn2,sn1));
	flag=pDb->Execute(SString::toFormat("update t_ssp_svglib_item set svgtype_sn=%d where svgtype_sn=%d",sn1,sn1+10000));
	if(flag==false)
	{
		ui.tableWidget_type->blockSignals(false);
		return;
	}
	ui.tableWidget_type->item(curRow,1)->setText(text2);
	ui.tableWidget_type->item(curRow+1,1)->setText(text1);
	ui.tableWidget_type->setCurrentItem(ui.tableWidget_type->item(curRow+1,1));
	ui.tableWidget_type->blockSignals(false);
}
void view_svglib_edit::onPushButton_copy_svg()
{
	ui.tableWidget_svg->blockSignals(true);
	int curRow=ui.tableWidget_svg->currentRow();
	if(curRow<0)
	{
		ui.tableWidget_svg->blockSignals(false);
		return;
	}
	bool ok;  
	int cpyNum= QInputDialog::getInteger(this,tr("复制个数"),  
		tr("输入复制个数:"), 1,1,150,1,&ok); 
	if(!ok)
	{
		ui.tableWidget_svg->blockSignals(false);
		return;
	}
	if(cpyNum<1)
	{
		ui.tableWidget_svg->blockSignals(false);
		return;
	}
	int maxSvgsn=pDb->SelectIntoI("select max(svg_sn) from t_ssp_svglib_item");
	maxSvgsn++;

	for(int i=0;i<cpyNum;i++)
	{
		bool flag=pDb->Execute(SString::toFormat("insert into t_ssp_svglib_item (svg_sn,svgtype_sn,svg_name,svg_file) \
												values (%d,(select svgtype_sn from t_ssp_svglib_item as x where svg_sn=%s),\
												(select svg_name from t_ssp_svglib_item as x where svg_sn=%s),\
												(select svg_file from t_ssp_svglib_item as x where svg_sn=%s))",\
												 maxSvgsn,ui.tableWidget_svg->item(curRow,0)->text().toStdString().c_str(),ui.tableWidget_svg->item(curRow,0)->text().toStdString().c_str(),ui.tableWidget_svg->item(curRow,0)->text().toStdString().c_str()));
		
		if(flag==false)
		{
			QMessageBox::warning(this,tr("警告"),tr("在数据库复制失败"),QMessageBox::Ok);
			ui.tableWidget_svg->blockSignals(false);
			return;
		}
		ui.tableWidget_svg->insertRow(ui.tableWidget_svg->rowCount());
		QTableWidgetItem *item=new QTableWidgetItem();
		item->setText(QString::number(maxSvgsn));
		ui.tableWidget_svg->setItem(ui.tableWidget_svg->rowCount()-1,0,item);

		item=new QTableWidgetItem();
		item->setText(ui.tableWidget_svg->item(curRow,1)->text());
		ui.tableWidget_svg->setItem(ui.tableWidget_svg->rowCount()-1,1,item);

		item=new QTableWidgetItem();
		item->setText(ui.tableWidget_svg->item(curRow,2)->text());
		ui.tableWidget_svg->setItem(ui.tableWidget_svg->rowCount()-1,2,item);
		maxSvgsn++;
	}
	ui.tableWidget_svg->blockSignals(false);
}
void view_svglib_edit::onPushButton_up_svg()
{

	ui.tableWidget_svg->blockSignals(true);
	int curRow;
	curRow=ui.tableWidget_svg->currentRow();
	if(curRow==-1||curRow==0)
	{
		ui.tableWidget_svg->blockSignals(false);
		return;
	}
	int sn1=ui.tableWidget_svg->item(curRow-1,0)->text().toInt();
	int sn2=ui.tableWidget_svg->item(curRow,0)->text().toInt();
	QString text1=ui.tableWidget_svg->item(curRow-1,2)->text();
	QString text2=ui.tableWidget_svg->item(curRow,2)->text();
	bool flag=pDb->Execute(SString::toFormat("update t_ssp_svglib_item set svg_sn=case when svg_sn=%d then %d when svg_sn=%d then %d else svg_sn end where svg_sn in (%d,%d)",sn1,sn2+10000,sn2,sn1,sn1,sn2));
	flag=pDb->Execute(SString::toFormat("update t_ssp_svglib_item set svg_sn=%d where svg_sn=%d",sn2,sn2+10000));
	if(flag==false)
	{
		ui.tableWidget_svg->blockSignals(false);
		return;
	}
	ui.tableWidget_svg->item(curRow-1,2)->setText(text2);
	ui.tableWidget_svg->item(curRow,2)->setText(text1);
	ui.tableWidget_svg->setCurrentItem(ui.tableWidget_svg->item(curRow-1,2));
	ui.tableWidget_svg->blockSignals(false);
}
void view_svglib_edit::onPushButton_down_svg()
{
	ui.tableWidget_svg->blockSignals(true);
	int curRow;
	curRow=ui.tableWidget_svg->currentRow();
	if(curRow==-1||curRow==ui.tableWidget_svg->rowCount()-1)
	{
		ui.tableWidget_svg->blockSignals(false);
		return;
	}
	int sn1=ui.tableWidget_svg->item(curRow,0)->text().toInt();
	int sn2=ui.tableWidget_svg->item(curRow+1,0)->text().toInt();
	QString text1=ui.tableWidget_svg->item(curRow,2)->text();
	QString text2=ui.tableWidget_svg->item(curRow+1,2)->text();
	bool flag=pDb->Execute(SString::toFormat("update t_ssp_svglib_item set svg_sn=case when svg_sn=%d then %d when svg_sn=%d then %d else svg_sn end where svg_sn in (%d,%d)",sn1,sn2+10000,sn2,sn1,sn1,sn2));
	flag=pDb->Execute(SString::toFormat("update t_ssp_svglib_item set svg_sn=%d where svg_sn=%d",sn2,sn2+10000));
	if(flag==false)
	{
		ui.tableWidget_svg->blockSignals(false);
		return;
	}
	ui.tableWidget_svg->item(curRow,2)->setText(text2);
	ui.tableWidget_svg->item(curRow+1,2)->setText(text1);
	ui.tableWidget_svg->setCurrentItem(ui.tableWidget_svg->item(curRow+1,2));
	ui.tableWidget_svg->blockSignals(false);
}
void view_svglib_edit::onPushButton_othtype_svg()
{
	if(ui.tableWidget_svg->selectedRanges().size()==0)
		return;
	m_inputDlg->exec();
	int typesn=m_inputDlg->getNum();
	if(!m_inputDlg->isOk())
		return;
	QList<int> snList;
	for(int count=0;count<ui.tableWidget_svg->selectedRanges().size();count++)
	{
		QTableWidgetSelectionRange sels=ui.tableWidget_svg->selectedRanges().at(count);
		if(sels.rowCount()>0)
		{
			for(int i=sels.topRow();i<=sels.bottomRow();i++)
			{
				snList.append(ui.tableWidget_svg->item(i,0)->text().toInt());
			}
		}
	}

	bool flag=true;
	for(int i=0;i<snList.size();i++)
	{
		flag=pDb->Execute(SString::toFormat("update t_ssp_svglib_item set svgtype_sn=%d where svg_sn=%d",typesn,snList.at(i)));
	}
	if(flag==false)
	{
		QMessageBox::warning(this,tr("警告"),tr("移动失败"),QMessageBox::Ok);
		return;
	}
	ui.tableWidget_svg->blockSignals(true);
	for(int i=0;i<snList.size();i++)
	{
		for(int j=0;j<ui.tableWidget_svg->rowCount();j++)
		{
			if(ui.tableWidget_svg->item(j,0)->text().toInt()==snList.at(i))
			{
				ui.tableWidget_svg->removeRow(j);
				break;
			}
		}
	}
	ui.tableWidget_svg->blockSignals(false);
}
void view_svglib_edit::onPushButton_del()
{
	ui.tableWidget_type->blockSignals(true);
	ui.tableWidget_svg->blockSignals(true);
	int curRow;
	ui.groupBox_2->setTitle("图元集");
	if(ui.tableWidget_type->rowCount() == 0)
	{
		ui.tableWidget_type->blockSignals(false);
		ui.tableWidget_svg->blockSignals(false);
		return;
	}
	curRow=ui.tableWidget_type->currentRow();
	if(curRow<0)
	{
		ui.tableWidget_type->blockSignals(false);
		ui.tableWidget_svg->blockSignals(false);
		return;
	}
	int i_ret = QMessageBox::warning(this,tr("警告"),tr("是否删除图元类型第")+QString::number(curRow+1)+tr("行？"),QMessageBox::Yes,QMessageBox::No);
	if (i_ret==QMessageBox::No)
	{
		ui.tableWidget_type->blockSignals(false);
		ui.tableWidget_svg->blockSignals(false);
		return;
	}
	int curNo=ui.tableWidget_type->item(curRow,0)->text().toInt();
	if(ui.tableWidget_svg->rowCount()>0)
	{
		if(QMessageBox::No==QMessageBox::warning(this,tr("警告"),tr("该类型下存在")+QString::number(ui.tableWidget_svg->rowCount())+tr("个图元，是否删除？"),QMessageBox::Ok,QMessageBox::No))
		{
			ui.tableWidget_type->blockSignals(false);
			ui.tableWidget_svg->blockSignals(false);
			return;
		}
	}
	bool flag=pDb->Execute(SString::toFormat("delete from t_ssp_svglib_type where svgtype_sn=%s",\
		QString::number(curNo).toStdString().c_str()));
	if(flag==false)
	{
		QMessageBox::warning(this,tr("警告"),tr("数据库删除图元类型失败"),QMessageBox::Ok);
		ui.tableWidget_type->blockSignals(false);
		ui.tableWidget_svg->blockSignals(false);
		return;
	}
	flag=pDb->Execute(SString::toFormat("delete from t_ssp_svglib_item where svgtype_sn=%s",\
		QString::number(curNo).toStdString().c_str()));
// 	if(flag==false)
// 	{
// 		QMessageBox::warning(this,tr("警告"),tr("数据库删除图元类型对应图元失败"),QMessageBox::Ok);
// 		return;
// 	}
	
	ui.tableWidget_type->removeRow(curRow);
	
	ui.tableWidget_svg->clear();
	while(ui.tableWidget_svg->rowCount()!=0)
		ui.tableWidget_svg->removeRow(0);
	QStringList header;
	header<<tr("序号")<<tr("类型")<<tr("名称");//<<tr("图元内容");
	ui.tableWidget_svg->setColumnCount(3);
	ui.tableWidget_svg->setHorizontalHeaderLabels(header);
	header.clear();
	m_svgTable.clear();
	if(ui.tableWidget_type->rowCount()>0)
		showSvgInfo(ui.tableWidget_type->currentItem(),NULL);
	ui.tableWidget_type->blockSignals(false);
	ui.tableWidget_svg->blockSignals(false);
}
void view_svglib_edit::onPushButton_add_svg()
{
	ui.tableWidget_svg->blockSignals(true);
	int curTypeRow=ui.tableWidget_type->currentRow();
	if(curTypeRow<0)
	{
		QMessageBox::warning(this,tr("警告"),tr("类型没有选择"),QMessageBox::Ok);
		ui.tableWidget_svg->blockSignals(false);
		return;
	}
	int curTypeNo=ui.tableWidget_type->item(curTypeRow,0)->text().toInt();
	if(curTypeNo<0)
		return;
	int curRow=ui.tableWidget_svg->rowCount();
	if(curRow<0)
		curRow=0;

	const char* buff="";
	SString sSave="";
	if (m_svgEditWnd==NULL)
	{
		m_svgEditWnd=new DynamicSSvgEditorWnd();
	}
	m_svgEditWnd->setData(pDb,"",(char *)buff);
	m_svgEditWnd->setWindowState(Qt::WindowMaximized);
	m_svgEditWnd->exec();
	m_svgEditWnd->getSaveBuff(sSave);
	delete m_svgEditWnd;
	m_svgEditWnd=NULL;

	int flag2=pDb->RetrieveRecordset(SString::toFormat("select svg_sn,svgtype_sn,svg_name,svg_file from t_ssp_svglib_item order by svg_sn"),m_svgTable);

	if(flag2<0)
	{
		QMessageBox::warning(this,tr("警告"),tr("数据库读取图元出错"),QMessageBox::Ok);
		ui.tableWidget_svg->blockSignals(false);
		return;
	}

	bool flag1=true;
	int num=1;
	while(flag1)
	{
		flag1=false;
		for(int row=0;row<m_svgTable.GetRows();row++)
		{
			if(m_svgTable.GetValue(row,0).toInt()==num)
			{
				num++;
				flag1=true;
			}
		}
	}
	bool flag=pDb->Execute(SString::toFormat("insert into t_ssp_svglib_item \
											 (svg_sn,svgtype_sn,svg_name)\
											 values (%s, %s,'新图元')",QString::number(num).toStdString().c_str(),\
											 QString::number(curTypeNo).toStdString().c_str()));
	if(flag==false)
	{
		QMessageBox::warning(this,tr("警告"),tr("数据库添加图元失败"),QMessageBox::Ok);
		ui.tableWidget_svg->blockSignals(false);
		return;
	}
	if(sSave!="")
		flag=pDb->UpdateLobFromMem("t_ssp_svglib_item","svg_file",SString::toFormat("svg_sn=%s",QString::number(num).toStdString().c_str()),(unsigned char*)sSave.data(),sSave.size());
	if(flag==false)
	{
		QMessageBox::warning(this,tr("警告"),tr("数据库添加图元内容失败"),QMessageBox::Ok);
		ui.tableWidget_svg->blockSignals(false);
		return;
	}

	ui.tableWidget_svg->insertRow(curRow);
	QTableWidgetItem *item=new QTableWidgetItem();
	item->setText(QString::number(num));
	item->setFlags(item->flags() & ~Qt::ItemIsEditable);
	ui.tableWidget_svg->setItem(curRow,0,item);

	item=new QTableWidgetItem();
	item->setText(QString::number(curTypeNo));
	item->setFlags(item->flags() & ~Qt::ItemIsEditable);
	ui.tableWidget_svg->setItem(curRow,1,item);

	item=new QTableWidgetItem("新图元");
	ui.tableWidget_svg->setItem(curRow,2,item);

// 	item=new QTableWidgetItem();
// 	item->setText(sSave.data());
// 	item->setFlags(item->flags() & ~Qt::ItemIsEditable);
// 	ui.tableWidget_svg->setItem(curRow,3,item);
	ui.tableWidget_svg->blockSignals(false);
}
void view_svglib_edit::onPushButton_edit_svg()
{
	ui.tableWidget_svg->blockSignals(true);
	int curRow;
	curRow=ui.tableWidget_svg->currentRow();
	
	if(curRow<0)
	{
		ui.tableWidget_svg->blockSignals(false);
		return;
	}
	int curNo=ui.tableWidget_svg->item(curRow,0)->text().toInt();
	unsigned char* buff=NULL;
	int buflen=0;
	bool flag=pDb->ReadLobToMem("t_ssp_svglib_item","svg_file",SString::toFormat("svg_sn=%d",curNo),buff,buflen);
	if(flag==false)
	{
		QMessageBox::warning(this,tr("警告"),tr("数据库读取图元数据失败"),QMessageBox::Ok);
		ui.tableWidget_svg->blockSignals(false);
		return;
	}

	//int len=0;
	SString sSave="";
	if (m_svgEditWnd==NULL)
	{
		m_svgEditWnd=new DynamicSSvgEditorWnd();
	}
	//m_svgEditWnd->set
	m_svgEditWnd->setData(pDb,"",(char *)buff);
	m_svgEditWnd->setWindowState(Qt::WindowMaximized);
	m_svgEditWnd->exec();
	m_svgEditWnd->getSaveBuff(sSave);
 	delete m_svgEditWnd;
 	m_svgEditWnd=NULL;
	if(sSave!="")
		flag=pDb->UpdateLobFromMem("t_ssp_svglib_item","svg_file",SString::toFormat("svg_sn=%s",ui.tableWidget_svg->item(curRow,0)->text().toStdString().c_str()),(unsigned char*)sSave.data(),sSave.size());
	else
	{
		ui.tableWidget_svg->blockSignals(false);
		pDb->FreeLobMem(buff);
		return;
	}
	if(flag==false)
	{
		QMessageBox::warning(this,tr("警告"),tr("数据库修改图元内容失败"),QMessageBox::Ok);
		ui.tableWidget_svg->blockSignals(false);
		pDb->FreeLobMem(buff);
		return;
	}
	//ui.tableWidget_svg->item(curRow,3)->setText(sSave.data());
	ui.tableWidget_svg->blockSignals(false);
	pDb->FreeLobMem(buff);
}
void view_svglib_edit::onPushButton_del_svg()
{
	int curRow;
	curRow=ui.tableWidget_svg->currentRow();
	if(curRow<0)
		return;
	int i_ret = QMessageBox::warning(this,tr("警告"),tr("是否删除svg第")+QString::number(curRow+1)+tr("行？"),QMessageBox::Yes,QMessageBox::No);
	if (i_ret==QMessageBox::No)
	{
		return;
	}
	bool flag=true;
	int curNo=ui.tableWidget_svg->item(curRow,0)->text().toInt();
	flag=pDb->Execute(SString::toFormat("delete from t_ssp_svglib_item where svg_sn=%s",QString::number(curNo).toStdString().c_str()));
	if(flag==false)
	{
		QMessageBox::warning(this,tr("警告"),tr("数据库删除图元失败"),QMessageBox::Ok);
		return;
	}
	ui.tableWidget_svg->blockSignals(true);
	ui.tableWidget_svg->removeRow(curRow);
	ui.tableWidget_svg->blockSignals(false);
}
void view_svglib_edit::svgFiletoXmlTxt()
{
	SXmlConfig &pCfg = *SSvgLibraryWnd::GetGlobalXmlConfig();
	SRecordset m_svgType;//主表信息
	SRecordset m_svg;//svg数
	QMap<int,SBaseConfig *>rootMap;
	SDatabaseOper *pDb=DB;
	pCfg.clear();
	pDb->RetrieveRecordset("select svgtype_sn,svgtype_name from t_ssp_svglib_type order by svgtype_sn",m_svgType);
	pDb->RetrieveRecordset("select svg_sn,svgtype_sn,svg_name,svg_file from t_ssp_svglib_item order by svg_sn",m_svg);
	pCfg.SetNodeName("svg-library");
	QString headSvg="<\?xml version=\"1.0\" encoding=\"GBK\"\?>\n<!--\nGenerated by : Skt-SVG Editor\nDate         : 2015-09-26 12:39:59\nVersion      : 1.0.4.18\n-->\n<svg width=\"\" height=\"\" >";
	QString rearSvg="</svg>";
	for(int i=0;i<m_svgType.GetRows();i++)
	{
		SBaseConfig *pCfgChild=pCfg.AddChildNode("svg-type");
		pCfgChild->SetAttribute("name",m_svgType.GetValue(i,1));
		rootMap.insert(m_svgType.GetValue(i,0).toInt(),pCfgChild);
	}
	for(int i=0;i<m_svg.GetRows();i++)
	{
		int curNo=m_svg.GetValue(i,0).toInt();
		SBaseConfig *pCfgChild=rootMap.value(m_svg.GetValue(i,1).toInt());
		if(pCfgChild!=NULL)
		{
			SBaseConfig *svgNode=pCfgChild->AddChildNode("svg");
			unsigned char* buff=NULL;
			int buflen;
			bool flag=pDb->ReadLobToMem("t_ssp_svglib_item","svg_file",SString::toFormat("svg_sn=%d",curNo),(BYTE* &)buff,buflen);
			if(flag==false || buff == NULL)
			{
				QMessageBox::warning(NULL,tr("警告"),tr("数据库读取图元数据失败"),QMessageBox::Ok);
				return;
			}
			//QString svgFile=buff;
			//svgFile.remove(QRegExp("<\?xml[^>]*>")).remove(QRegExp("<![^>]*>")).remove("<rect uri=\"\" caption=\"\" x=\"0\" y=\"0\" z=\"\" width=\"1280\" height=\"1024\" stroke=\"\" stroke-width=\"\" stroke-dasharray=\"\" fill=\"#ffffff\" />").remove("<rect uri=\"\" caption=\"\" x=\"0\" y=\"0\" z=\"\" width=\"1280\" height=\"1024\" stroke=\"#000000\" stroke-width=\"1\" stroke-dasharray=\"0\" fill=\"#000000\" />").remove(QRegExp("<svg[^>]*>")).remove(rearSvg).remove("<?");
			char *pSvg = strstr((char*)buff,"<defs>");
			if(pSvg == NULL)
			{
				pSvg = strstr((char*)buff,"<rect");
				if(pSvg != NULL)
					pSvg = strstr(pSvg,"/>");
				if(pSvg != NULL)
					pSvg += 2;
			}
			else
			{
				int p1 = -1,p2 = -1;
				char *pStr1;
				pStr1 = strstr((char*)buff,"<rect");
				if(pStr1 != NULL)
				{
					p1 = pStr1-(char*)buff;
					pStr1 = strstr(pStr1,"/>");
					if(pStr1 != NULL)
						p2 = pStr1+2-(char*)buff;
				}
				if(p1 > 0 && p2 > 0)
				{
					for(int i=p1;i<p2;i++)
						buff[i] = ' ';
				}
			}
			if(pSvg != NULL)
			{
				char *pTail = strstr(pSvg,"</svg>");
				if(pTail != NULL)
					pTail[0] = '\0';
				svgNode->SetAttribute("name",m_svg.GetValue(i,2));
				svgNode->SetNodeValue(pSvg/*svgFile.toStdString().data()*/);
				//svgNode->SetNodeValue("<![CDATA["+svgNode->GetNodeValue().trim()+"]]>");
			}
			pDb->FreeLobMem(buff);
		}
	
	}
// 	SString xmlFile;
// 	pCfg.SaveConfigToText(xmlFile);
// 	SSvgLibraryWnd::LoadDefaltByXmlText(xmlFile.data());
}
void view_svglib_edit::onPushButton_app()
{
	svgFiletoXmlTxt();
}
void view_svglib_edit::onPushButton_import()
{

	QString path=QFileDialog::getOpenFileName(this,"打开文件","./../conf/sys_svg_library.xml","*.xml",0,0);
	if(path=="")
		return;
	SXmlConfig xml;
	if(!xml.ReadConfig( path.toStdString()))
	{
		QMessageBox::warning(this,tr("警告"),tr("xml文件读取失败"),QMessageBox::Ok);
		LOGERROR("Load Datawindow config error!file=%s", path.toStdString().data());
		return;
	}

	SBaseConfig *pDW=&xml;
	if(pDW->GetNodeName() != "svg-library")
	{
		QMessageBox::warning(this,tr("警告"),tr("xml文件根节点不是svg-library"),QMessageBox::Ok);
		LOGERROR("Datawindow root node is not 'svg-library'!");
		return;
	}
	int i,cnt;
	cnt = pDW->GetChildCount("svg-type");
	
	if(QMessageBox::Ok==QMessageBox::information(this,tr("消息"),tr("导入将清除原有数据，是否继续"),QMessageBox::Ok,QMessageBox::No))
	{
		ui.tableWidget_type->blockSignals(true);
		ui.tableWidget_svg->blockSignals(true);
		//初始化集表格
		ui.groupBox_2->setTitle("图元集");
		ui.tableWidget_svg->clear();
		while(ui.tableWidget_svg->rowCount()!=0)
			ui.tableWidget_svg->removeRow(0);
		QStringList header;
		header<<tr("序号")<<tr("类型")<<tr("名称");//<<tr("图元内容");
		ui.tableWidget_svg->setColumnCount(3);
		ui.tableWidget_svg->setHorizontalHeaderLabels(header);
		header.clear();
		m_svgTable.clear();
		bool flag=pDb->Execute(SString::toFormat("delete from t_ssp_svglib_item"));
		if(flag==false)
		{
			//QMessageBox::warning(this,tr("警告"),tr("数据库清理图元失败"),QMessageBox::Ok);
			//return;
		}

		//初始化集表格
		ui.tableWidget_type->clear();
		while(ui.tableWidget_type->rowCount()!=0)
			ui.tableWidget_type->removeRow(0);
		header<<tr("序号")<<tr("名称");
		ui.tableWidget_type->setColumnCount(2);
		ui.tableWidget_type->setHorizontalHeaderLabels(header);
		header.clear();
		m_typeTable.clear();
		flag=pDb->Execute(SString::toFormat("delete from t_ssp_svglib_type"));
		if(flag==false)
		{
			//QMessageBox::warning(this,tr("警告"),tr("数据库清理图元类型失败"),QMessageBox::Ok);
			//return;
		}
	}
	int flag2=pDb->RetrieveRecordset(SString::toFormat("select svg_sn,svgtype_sn,svg_name,svg_file from t_ssp_svglib_item order by svg_sn"),m_svgTable);
	if(flag2<0)
	{
		//QMessageBox::warning(this,tr("警告"),tr("数据库读取图元出错"),QMessageBox::Ok);
		//return;
	}

	int svgCount=m_svgTable.GetRows();
	for(i=0;i<cnt;i++)
	{
		SBaseConfig *pRefCfg = pDW->GetChildNode(i,"svg-type");
		QString name = pRefCfg->GetAttribute("name").data();
		int rows = pRefCfg->GetChildCount("svg");

		bool flag1=true;
		int num=1;
		while(flag1)
		{
			flag1=false;
			for(int row=0;row<ui.tableWidget_type->rowCount();row++)
			{
				if(ui.tableWidget_type->item(row,0)->text().toInt()==num)
				{
					num++;
					flag1=true;
				}
			}
		}
		bool flag=pDb->Execute(SString::toFormat("insert into t_ssp_svglib_type (svgtype_sn,svgtype_name) \
												 values (%s,'%s')",QString::number(num).toStdString().c_str(),name.toStdString().c_str()));
		if(flag==false)
		{
			QMessageBox::warning(this,tr("警告"),tr("数据库插入图元类型失败"),QMessageBox::Ok);
			ui.tableWidget_type->blockSignals(false);
			ui.tableWidget_svg->blockSignals(false);
			return;
		}
		for(int r=0;r<rows;r++)
		{
			SBaseConfig *pR = pRefCfg->GetChildNode(r,"svg");
			QString svgName=pR->GetAttribute("name").data();
			QString headSvg="<\?xml version=\"1.0\" encoding=\"GBK\"\?>\n<svg width=\"1280\" height=\"1024\" fill=\"#ffffff\">\n<rect uri=\"\" caption=\"\" x=\"0\" y=\"0\" z=\"\" width=\"1280\" height=\"1024\" stroke=\"\" stroke-width=\"\" stroke-dasharray=\"\" fill=\"#ffffff\" />";
			QString rearSvg="</svg>";
			QString svgFile=headSvg+pR->GetNodeValue().data()+rearSvg;
			bool flag=pDb->Execute(SString::toFormat("insert into t_ssp_svglib_item (svg_sn,svgtype_sn,svg_name)\
													 values (%s, %s,'%s')",QString::number(svgCount+1).toStdString().c_str(),\
													 QString::number(num).toStdString().c_str(),svgName.toStdString().c_str()));
			if(flag==false)
			{
				QMessageBox::warning(this,tr("警告"),tr("数据库插入图元失败"),QMessageBox::Ok);
				ui.tableWidget_type->blockSignals(false);
				ui.tableWidget_svg->blockSignals(false);
				return;
			}
			SString sSave=svgFile.toStdString().data();
			if(sSave!="")
				flag=pDb->UpdateLobFromMem("t_ssp_svglib_item","svg_file",SString::toFormat("svg_sn=%s",QString::number(svgCount+1).toStdString().c_str()),(unsigned char*)sSave.data(),sSave.size());
			if(flag==false)
			{
				QMessageBox::warning(this,tr("警告"),tr("数据库插入图元数据失败"),QMessageBox::Ok);
				ui.tableWidget_type->blockSignals(false);
				ui.tableWidget_svg->blockSignals(false);
				return;
			}
			svgCount++;
		}
		ui.tableWidget_type->insertRow(ui.tableWidget_type->rowCount());
		QTableWidgetItem *item=new QTableWidgetItem(QString::number(num));
		item->setFlags(item->flags() & ~Qt::ItemIsEditable);
		ui.tableWidget_type->setItem(ui.tableWidget_type->rowCount()-1,0,item);

		item=new QTableWidgetItem(name);
		ui.tableWidget_type->setItem(ui.tableWidget_type->rowCount()-1,1,item);
	}
	//refreshTypeTable();
	ui.tableWidget_type->blockSignals(false);
	ui.tableWidget_svg->blockSignals(false);
}
void view_svglib_edit::onPushButton_export()
{
	SvgOutputDlg dlg(this);
	dlg.exec();
}
void view_svglib_edit::editType(QTableWidgetItem *item)
{
	int col=item->column();
	int row=item->row();
	int curNo=ui.tableWidget_type->item(row,0)->text().toInt();
	if(col==1)
	{
		bool flag=pDb->Execute(SString::toFormat("update t_ssp_svglib_type set svgtype_name='%s' where svgtype_sn=%s",\
			item->text().toStdString().c_str(),QString::number(curNo).toStdString().c_str()));
		if(flag==false)
		{
			QMessageBox::warning(this,tr("警告"),tr("数据库编辑图元类型名称失败"),QMessageBox::Ok);
			return;
		}
	}
}
void view_svglib_edit::editSvg(QTableWidgetItem *item)
{
	int col=item->column();
	int row=item->row();
	int curNo=ui.tableWidget_svg->item(row,0)->text().toInt();
	if(col==2)
	{
		bool flag=pDb->Execute(SString::toFormat("update t_ssp_svglib_item set svg_name='%s' where svg_sn=%s",\
			item->text().toStdString().c_str(),QString::number(curNo).toStdString().c_str()));
		if(flag==false)
		{
			QMessageBox::warning(this,tr("警告"),tr("数据库编辑图元名称失败"),QMessageBox::Ok);
			return;
		}
	}
}
////////////////////////////////////////////////////////////////////////////////
SvgOutputDlg::SvgOutputDlg(QWidget *parent)
	: QDialog(parent)
{
	setupUi(this);
	pDb = DB;
	pDb->RetrieveRecordset("select svgtype_sn,svgtype_name from t_ssp_svglib_type order by svgtype_sn",m_svgType);
	pDb->RetrieveRecordset("select svg_sn,svgtype_sn,svg_name,svg_file from t_ssp_svglib_item order by svg_sn",m_svg);
	QMap<QString,QString> svgType;
	for(int i=0;i<m_svgType.GetRows();i++)
	{
		svgType.insert(m_svgType.GetValue(i,0).data(),m_svgType.GetValue(i,1).data());
	}
	treeWidget->setColumnCount(2);
	QStringList headers; 
	headers << tr("序号") << tr("名称"); 
	treeWidget->setHeaderLabels(headers);
	treeWidget->setColumnWidth(0,100);
	treeWidget->setColumnWidth(1,200);
	setWindowTitle("输出窗口");
	treeWidget->setAlternatingRowColors(true);
	connect(treeWidget,SIGNAL(itemClicked(QTreeWidgetItem *,int )),this,SLOT(tree_checked(QTreeWidgetItem *)));//勾选触发
	connect(pushButton,SIGNAL(clicked()),this,SLOT(onPushButtonAllChecked()));//勾选触发
	connect(pushButton_2,SIGNAL(clicked()),this,SLOT(onPushButtonAllUnchecked()));//勾选触发
	connect(pushButton_3,SIGNAL(clicked()),this,SLOT(onPushButtonOk()));//勾选触发
	connect(pushButton_4,SIGNAL(clicked()),this,SLOT(onPushButtonCancel()));//勾选触发
	QTreeWidgetItem *root =NULL;
	QTreeWidgetItem *leaf =NULL;

	for(int i=0;i<m_svgType.GetRows();i++)
	{
		QStringList tmp;
		tmp<<QString(m_svgType.GetValue(i,0))<<QString(m_svgType.GetValue(i,1));
		root = new QTreeWidgetItem(treeWidget, tmp); 
		root->setCheckState(0,Qt::Checked);
		rootMap.insert(m_svgType.GetValue(i,0).toInt(),root);
		treeWidget->insertTopLevelItem(i,root);
	}
	for(int i=0;i<m_svg.GetRows();i++)
	{
		QStringList tmp1;
		tmp1<<QString(m_svg.GetValue(i,0))<<QString("       "+m_svg.GetValue(i,2));
		QTreeWidgetItem *tmpRoot=rootMap.value(m_svg.GetValue(i,1).toInt());
		if(tmpRoot!=NULL)
		{
			leaf=new QTreeWidgetItem(tmpRoot, tmp1);
			leaf->setCheckState(0,Qt::Checked);
			tmpRoot->addChild(leaf);
		}

	}
}

SvgOutputDlg::~SvgOutputDlg()
{

}
void SvgOutputDlg::tree_checked(QTreeWidgetItem *item)
{
	if(item->parent()==NULL)//根节点
	{
		for(int i=0;i<item->childCount();i++)
		{
			if(item->checkState(0)==Qt::Checked)
				item->child(i)->setCheckState(0,Qt::Checked);
			else
				item->child(i)->setCheckState(0,Qt::Unchecked);
		}
	}
}
void SvgOutputDlg::onPushButtonAllChecked()//全选
{
	QMapIterator<int,QTreeWidgetItem*>rootMapIterator(rootMap);
	while(rootMapIterator.hasNext())
	{
		rootMapIterator.next();
		QTreeWidgetItem *tmpRoot=rootMapIterator.value();
		tmpRoot->setCheckState(0,Qt::Checked);
		for(int i=0;i<tmpRoot->childCount();i++)
		{
			tmpRoot->child(i)->setCheckState(0,Qt::Checked);
		}
	}
}
void SvgOutputDlg::onPushButtonAllUnchecked()//全不选
{
	QMapIterator<int,QTreeWidgetItem*>rootMapIterator(rootMap);
	while(rootMapIterator.hasNext())
	{
		rootMapIterator.next();
		QTreeWidgetItem *tmpRoot=rootMapIterator.value();
		tmpRoot->setCheckState(0,Qt::Unchecked);
		for(int i=0;i<tmpRoot->childCount();i++)
		{
			tmpRoot->child(i)->setCheckState(0,Qt::Unchecked);
		}

	}
}
//输出为xml
void SvgOutputDlg::onPushButtonOk()
{
	QString path=QFileDialog::getSaveFileName(this,"保存文件","./../conf/sys_svg_library.xml","*.xml",0,0);
	if(path=="")
		return;
	SXmlConfig pCfg;
	pCfg.SetNodeName("svg-library");
	QString headSvg="<\?xml version=\"1.0\" encoding=\"GBK\"\?>\n<svg width=\"\" height=\"\" >";
	QString rearSvg="</svg>";
	QMapIterator<int,QTreeWidgetItem*>rootMapIterator(rootMap);
	while(rootMapIterator.hasNext())
	{
		rootMapIterator.next();
		QTreeWidgetItem *tmpRoot=rootMapIterator.value();
		bool isNodeExist=false;
		SBaseConfig *pCfgChild=NULL;
		for(int i=0;i<tmpRoot->childCount();i++)
		{
			if(tmpRoot->child(i)->checkState(0)==Qt::Checked)
			{
				if(isNodeExist==false)
				{
					pCfgChild=pCfg.AddChildNode("svg-type");
					pCfgChild->SetAttribute("name",tmpRoot->text(1).toStdString().data());
					isNodeExist=true;
				}
				int curNo=tmpRoot->child(i)->text(0).toInt();
				SBaseConfig *svgNode=pCfgChild->AddChildNode("svg");
				unsigned char* buff=NULL;
				int buflen;
				bool flag=pDb->ReadLobToMem("t_ssp_svglib_item","svg_file",SString::toFormat("svg_sn=%d",curNo),(BYTE* &)buff,buflen);
				if(flag==false || buff == NULL)
				{
					QMessageBox::warning(this,tr("警告"),tr("数据库读取图元数据失败"),QMessageBox::Ok);
					return;
				}
				char *pSvg = strstr((char*)buff,"<defs>");
				if(pSvg == NULL)
				{
					pSvg = strstr((char*)buff,"<rect");
					if(pSvg != NULL)
						pSvg = strstr(pSvg,"/>");
					if(pSvg != NULL)
						pSvg += 2;
				}
				else
				{
					int p1 = -1,p2 = -1;
					char *pStr1;
					pStr1 = strstr((char*)buff,"<rect");
					if(pStr1 != NULL)
					{
						p1 = pStr1-(char*)buff;
						pStr1 = strstr(pStr1,"/>");
						if(pStr1 != NULL)
							p2 = pStr1+2-(char*)buff;
					}
					if(p1 > 0 && p2 > 0)
					{
						for(int i=p1;i<p2;i++)
							buff[i] = ' ';
					}
				}
				if(pSvg != NULL)
				{
					char *pTail = strstr(pSvg,"</svg>");
					if(pTail != NULL)
						pTail[0] = '\0';
					svgNode->SetAttribute("name",pDb->SelectInto(SString::toFormat("select svg_name from t_ssp_svglib_item where svg_sn=%d",curNo)));
					svgNode->SetNodeValue(pSvg/*svgFile.toStdString().data()*/);
					//svgNode->SetNodeValue("<![CDATA["+svgNode->GetNodeValue().trim()+"]]>");
				}
				pDb->FreeLobMem(buff);
			}
		}
	}

	if(pCfg.SaveConfig(path.toStdString()))
		QMessageBox::information(this,tr("提示"),tr("导出成功"),QMessageBox::Ok);
	else
		QMessageBox::information(this,tr("提示"),tr("导出失败"),QMessageBox::Ok);
	
	close();
}
void SvgOutputDlg::onPushButtonCancel()
{
	close();
}

void SvgOutputDlg::setupUi(QDialog *SvgOutputDlg)
{
	if (SvgOutputDlg->objectName().isEmpty())
		SvgOutputDlg->setObjectName(QString::fromUtf8("SvgOutputDlg"));
	SvgOutputDlg->resize(569, 386);
	gridLayout_2 = new QGridLayout(SvgOutputDlg);
	gridLayout_2->setSpacing(6);
	gridLayout_2->setContentsMargins(11, 11, 11, 11);
	gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
	verticalLayout = new QVBoxLayout();
	verticalLayout->setSpacing(6);
	verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
	groupBox = new QGroupBox(SvgOutputDlg);
	groupBox->setObjectName(QString::fromUtf8("groupBox"));
	gridLayout = new QGridLayout(groupBox);
	gridLayout->setSpacing(4);
	gridLayout->setContentsMargins(0, 0, 0, 0);
	gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
	treeWidget = new QTreeWidget(groupBox);
	QTreeWidgetItem *__qtreewidgetitem = new QTreeWidgetItem();
	__qtreewidgetitem->setText(0, QString::fromUtf8("1"));
	treeWidget->setHeaderItem(__qtreewidgetitem);
	treeWidget->setObjectName(QString::fromUtf8("treeWidget"));

	gridLayout->addWidget(treeWidget, 0, 0, 1, 1);


	verticalLayout->addWidget(groupBox);

	horizontalLayout_3 = new QHBoxLayout();
	horizontalLayout_3->setSpacing(6);
	horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
	horizontalLayout_2 = new QHBoxLayout();
	horizontalLayout_2->setSpacing(6);
	horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
	pushButton = new QPushButton(SvgOutputDlg);
	pushButton->setObjectName(QString::fromUtf8("pushButton"));
	QIcon icon;
	icon.addFile(QString::fromUtf8(":/ssp_gui/Resources/20070402165933688.gif"), QSize(), QIcon::Normal, QIcon::Off);
	pushButton->setIcon(icon);

	horizontalLayout_2->addWidget(pushButton);

	pushButton_2 = new QPushButton(SvgOutputDlg);
	pushButton_2->setObjectName(QString::fromUtf8("pushButton_2"));
	QIcon icon1;
	icon1.addFile(QString::fromUtf8(":/ssp_gui/Resources/20070402165933865.gif"), QSize(), QIcon::Normal, QIcon::Off);
	pushButton_2->setIcon(icon1);

	horizontalLayout_2->addWidget(pushButton_2);


	horizontalLayout_3->addLayout(horizontalLayout_2);

	horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

	horizontalLayout_3->addItem(horizontalSpacer);

	horizontalLayout = new QHBoxLayout();
	horizontalLayout->setSpacing(6);
	horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
	pushButton_3 = new QPushButton(SvgOutputDlg);
	pushButton_3->setObjectName(QString::fromUtf8("pushButton_3"));
	QIcon icon2;
	icon2.addFile(QString::fromUtf8(":/ssp_gui/Resources/export.gif"), QSize(), QIcon::Normal, QIcon::Off);
	pushButton_3->setIcon(icon2);

	horizontalLayout->addWidget(pushButton_3);

	pushButton_4 = new QPushButton(SvgOutputDlg);
	pushButton_4->setObjectName(QString::fromUtf8("pushButton_4"));
	QIcon icon3;
	icon3.addFile(QString::fromUtf8(":/ssp_gui/Resources/del.gif"), QSize(), QIcon::Normal, QIcon::Off);
	pushButton_4->setIcon(icon3);

	horizontalLayout->addWidget(pushButton_4);


	horizontalLayout_3->addLayout(horizontalLayout);


	verticalLayout->addLayout(horizontalLayout_3);


	gridLayout_2->addLayout(verticalLayout, 0, 0, 1, 1);


	retranslateUi(SvgOutputDlg);

	QMetaObject::connectSlotsByName(SvgOutputDlg);
} // setupUi

void SvgOutputDlg::retranslateUi(QDialog *SvgOutputDlg)
{
	SvgOutputDlg->setWindowTitle(QApplication::translate("SvgOutputDlg", "SvgOutputDlg", 0, QApplication::UnicodeUTF8));
	groupBox->setTitle(QApplication::translate("SvgOutputDlg", "\345\257\274\345\207\272\345\210\227\350\241\250", 0, QApplication::UnicodeUTF8));
	pushButton->setText(QApplication::translate("SvgOutputDlg", "\345\205\250\351\200\211(&A)", 0, QApplication::UnicodeUTF8));
	pushButton_2->setText(QApplication::translate("SvgOutputDlg", "\345\205\250\344\270\215\351\200\211(&D)", 0, QApplication::UnicodeUTF8));
	pushButton_3->setText(QApplication::translate("SvgOutputDlg", "\345\257\274\345\207\272(&O)", 0, QApplication::UnicodeUTF8));
	pushButton_4->setText(QApplication::translate("SvgOutputDlg", "\345\217\226\346\266\210(&C)", 0, QApplication::UnicodeUTF8));
} // retranslateUi

//////////////////////////////////////////////////
inputNum1::inputNum1(QWidget *parent)
	: QDialog(parent)
{
	m_ok=false;
	pDb=DB;
	pDb->RetrieveRecordset("select svgtype_sn,svgtype_name from t_ssp_svglib_type order by svgtype_sn",m_sets);
	label=new QLabel(this);
	label->setText(tr("选择移动的序号"));
	cbo=new QComboBox(this);
	QHBoxLayout *hb1=new QHBoxLayout();
	QHBoxLayout *hb2=new QHBoxLayout();
	QVBoxLayout *vb1=new QVBoxLayout();
	hb1->addWidget(label);
	hb1->addWidget(cbo);
	hb1->addStretch();
	hb1->setMargin(0);
	for(int i=0;i<m_sets.GetRows();i++)
	{
		cbo->addItem(m_sets.GetValue(i,1).data());
	}
	pushButton=new QPushButton(tr("确认(&O)"),this);
	pushButton2=new QPushButton(tr("取消(&C)"),this);
	QIcon icon;
	icon.addFile(QString::fromUtf8(":/ssp_gui/Resources/save.gif"), QSize(), QIcon::Normal, QIcon::Off);
	pushButton->setIcon(icon);
	QIcon icon2;
	icon2.addFile(QString::fromUtf8(":/ssp_gui/Resources/del.gif"), QSize(), QIcon::Normal, QIcon::Off);
	pushButton2->setIcon(icon2);
	hb2->addWidget(pushButton);
	hb2->addWidget(pushButton2);
	hb2->addStretch();
	hb2->setMargin(4);
	vb1->addLayout(hb1);
	vb1->addLayout(hb2);
	vb1->setMargin(4);
	setLayout(vb1);
	setWindowTitle(tr("选择类型名称"));
	connect(pushButton,SIGNAL(clicked()),this,SLOT(onPushButtonPressed()));
	connect(pushButton2,SIGNAL(clicked()),this,SLOT(onPushButton2Pressed()));
}

inputNum1::~inputNum1()
{

}
void inputNum1::onPushButtonPressed()
{
	m_ok=true;
	hide();
}
void inputNum1::onPushButton2Pressed()
{
	m_ok=false;
	hide();
}
bool inputNum1::isOk()
{
	bool ok=m_ok;
	m_ok=false;
	return ok;
}
int inputNum1::getNum()
{
	QString tmp=cbo->currentText();
	for(int i=0;i<m_sets.GetRows();i++)
	{
		if(tmp==m_sets.GetValue(i,1).data())
			return m_sets.GetValue(i,0).toInt();
	}
	return -1;
}

///////////////////////////////////////////////