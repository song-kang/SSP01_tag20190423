#include "view_dw_condition_edit.h"

view_dw_condition_edit::view_dw_condition_edit(QWidget *parent)
	: CBaseView(parent)
{
	ui.setupUi(this);
	m_table=new QTableWidget(this);
	pDb=DB;
	initBaseData();
	
	refreshDataRef();
	initTable();
	QVBoxLayout *layout=new QVBoxLayout(this);
	QHBoxLayout *layoutBtn=new QHBoxLayout(this);
	addButton=new QPushButton(tr("添加"),this);
	QIcon icon;
	icon.addFile(QString::fromUtf8(":/ssp_gui/Resources/20070402165925169.gif"), QSize(), QIcon::Normal, QIcon::Off);
	addButton->setIcon(icon);
	addButton->setMaximumHeight(24);
	removeButton=new QPushButton(tr("删除"),this);
	QIcon icon1;
	icon1.addFile(QString::fromUtf8(":/ssp_gui/Resources/del.gif"), QSize(), QIcon::Normal, QIcon::Off);
	removeButton->setIcon(icon1);
	removeButton->setMaximumHeight(24);

	insertButton=new QPushButton(tr("插入"),this);
	QIcon icon2;
	icon1.addFile(QString::fromUtf8(":/ssp_gui/Resources/20070402165925169.gif"), QSize(), QIcon::Normal, QIcon::Off);
	insertButton->setIcon(icon);
	insertButton->setMaximumHeight(24);
	insertButton->setToolTip("插入选中行之前");

	layoutBtn->addWidget(addButton);
	layoutBtn->addWidget(removeButton);
	layoutBtn->addWidget(insertButton);
	layoutBtn->addStretch();
	layout->addLayout(layoutBtn);
	layout->addWidget(m_table);
	setLayout(layout);
	m_iNo=-1;
	connect(addButton,SIGNAL(clicked()),this,SLOT(onAddButton()));
	connect(removeButton,SIGNAL(clicked()),this,SLOT(onRemoveButton()));
	connect(insertButton,SIGNAL(clicked()),this,SLOT(onInsertButton()));
	connect(m_table,SIGNAL(itemChanged(QTableWidgetItem *)),this,SLOT(editItem(QTableWidgetItem *)));
	connect(m_table,SIGNAL(currentItemChanged(QTableWidgetItem *,QTableWidgetItem *)),this,SLOT(slotClickSel(QTableWidgetItem *,QTableWidgetItem *)));
}

view_dw_condition_edit::~view_dw_condition_edit()
{

}
void view_dw_condition_edit::initTable()
{
	QStringList header;
	header<<tr("条件序号")<<tr("条件名称")<<tr("比较类型")<<tr("输入类型")<<tr("是否可选")<<tr("引用名")<<tr("缺省值")<<tr("动态条件");
	m_table->setColumnCount(8);
	m_table->verticalHeader()->hide();
	m_table->setHorizontalHeaderLabels(header);
	m_table->setAlternatingRowColors(true);
	m_table->setItemDelegate(new dwconditionDelegate(this));
	m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
	m_table->setSelectionMode(QAbstractItemView::SingleSelection);
	m_table->setColumnWidth(0,60);
	m_table->setColumnWidth(1,100);
	m_table->setColumnWidth(2,120);
	m_table->setColumnWidth(3,120);
	m_table->setColumnWidth(4,100);
	m_table->setColumnWidth(5,100);
	m_table->setColumnWidth(6,100);
	m_table->setColumnWidth(7,100);
	//m_table->horizontalHeader()->setResizeMode(QHeaderView::ResizeToContents);
	SQt::SetTableWidgetStyle(m_table);
}
void view_dw_condition_edit::refreshTable(int Sno, QString strDataComBoxText, bool bCheckState)
{
	m_strComText = strDataComBoxText;
	m_bIsChecked = bCheckState;
	m_table->blockSignals(true);
	m_iNo=Sno;
	m_table->clearContents();
	m_table->setRowCount(0);
	removeButton->setEnabled(false);
	SRecordset conTable;
	SString sql;

	sql = SString::toFormat("select dw_sn,con_sn,con_name,cmp_type,input_type,is_option,ref_name,def_value,dyn_where from t_ssp_data_window_condition where dw_sn=%d order by con_sn",m_iNo);
	bool flag=pDb->RetrieveRecordset(sql,conTable);
	if(flag==false)
	{
		m_table->blockSignals(false);
		return;
	}

	for(int row = 0;row < conTable.GetRows(); row++)
	{
		m_table->insertRow(row);
		
		QTableWidgetItem *item=new QTableWidgetItem();//条件序号
		item->setText(conTable.GetValue(row,1).data());
		item->setFlags(item->flags()&~Qt::ItemIsEditable);
		m_table->setItem(row,0,item);

		item=new QTableWidgetItem();
		item->setText(conTable.GetValue(row,2).data());//条件名称
		m_table->setItem(row,1,item);

		item=new QTableWidgetItem();
		int conType=conTable.GetValue(row,3).toInt();//条件类型
		item->setText(m_cmpTypeMap[conTable.GetValue(row,3).data()]);
		m_table->setItem(row,2,item);

		item=new QTableWidgetItem();
		int inputType=conTable.GetValue(row,4).toInt();//输入类型
		item->setText(m_inputTypeMap[conTable.GetValue(row,4).data()]);
		m_table->setItem(row,3,item);

		item=new QTableWidgetItem();
		int isOpt=conTable.GetValue(row,5).toInt();//是否可选
		if(isOpt==1)
			item->setText(tr("是"));
		else if(isOpt==0)
			item->setText(tr("否"));
		m_table->setItem(row,4,item);

		item=new QTableWidgetItem();//引用名
		QString tmpStr=m_dataRefMap[conTable.GetValue(row,6).data()];

		item->setText(tmpStr);
		if(conType==1||conType==2||conType==5||conType==6||conType==7)
			item->setFlags(item->flags()&~Qt::ItemIsEditable);
		m_table->setItem(row,5,item);

		item=new QTableWidgetItem(conTable.GetValue(row,7).data());//缺省值
		if(inputType==1||inputType==2||inputType==3||inputType==6||inputType==7)
			item->setFlags(item->flags()&~Qt::ItemIsEditable);
		m_table->setItem(row,6,item);

		item=new QTableWidgetItem(conTable.GetValue(row,8).data());//动态条件
		m_table->setItem(row,7,item);
	}
	m_table->blockSignals(false);
	if (m_table->rowCount() > 0)
	{
		m_table->selectRow(0);
		removeButton->setEnabled(true);
	}
	m_table->resizeColumnsToContents();
}
void view_dw_condition_edit::setComboTextAndCheckState(QString strComText, bool bIsChecked)
{
	m_strComText = strComText;
	m_bIsChecked = bIsChecked;
}
void view_dw_condition_edit::refreshDataRef()
{
	SString sql = "select name,dw_desc from t_ssp_data_window where dstype=1 order by cls_name desc,dw_desc";
	m_refList.clear();
	int ret = pDb->RetrieveRecordset(sql,m_refList);
	if (ret < 0)
	{
		QMessageBox::warning(this,tr("警告"),tr("数据查询失败[%s]").arg(sql.data()),QMessageBox::Ok);
		return;
	}
	for(int i = 0; i < m_refList.GetRows(); i ++)
	{
		m_dataRefMap[m_refList.GetValue(i,0).data()] = m_refList.GetValue(i,1).data();
	}

}
void view_dw_condition_edit::initBaseData()
{
	m_cmpTypeMap.clear();
	m_cmpTypeMap["0"] = "无"; 
	m_cmpTypeMap["1"] = "字符比较"; 
	m_cmpTypeMap["2"] = "数值比较"; 
	m_cmpTypeMap["3"] = "字符集合"; 
	m_cmpTypeMap["4"] = "整数集合"; 
	m_cmpTypeMap["5"] = "日期比较"; 
	m_cmpTypeMap["6"] = "日期时间比较"; 
	m_cmpTypeMap["7"] = "世纪秒比较"; 

	m_inputTypeMap.clear();
	m_inputTypeMap["1"] = "任意字符串输入";
	m_inputTypeMap["2"] = "数值输入";
	m_inputTypeMap["3"] = "整数输入";
	m_inputTypeMap["4"] = "日期输入";
	m_inputTypeMap["5"] = "日期时间输入";
	m_inputTypeMap["6"] = "下拉框单选输入";
	m_inputTypeMap["7"] = "列表框多选输入";
}

void view_dw_condition_edit::onAddButton()
{
	int curRow=m_table->rowCount();
	if(curRow <= 0)
	{
		insertRow(0);

	} else {

		insertRow(curRow);
	}
	m_table->resizeColumnsToContents();
}
void view_dw_condition_edit::onInsertButton()
{
	if (m_table->rowCount() == 0)
	{
		insertRow(0);
		return;
	}

	int curSelRow = m_table->currentRow();
	if (curSelRow < 0)
	{
		QMessageBox::warning(this,tr("警告"),tr("没有选择指定行"),QMessageBox::Ok);
		return;
	} else {
		insertAppointRow(curSelRow);
	}
	m_table->resizeColumnsToContents();
}
void view_dw_condition_edit::onRemoveButton()
{
	if(m_table->rowCount()==0)
		return;
	int curRow=m_table->currentRow();
	if(curRow < 0)
		return;
	int i_ret = QMessageBox::warning(this,tr("警告"),tr("是否删除条件第")+QString::number(curRow+1)+tr("行？"),QMessageBox::Yes,QMessageBox::No);
	if (i_ret==QMessageBox::No)
	{
		return;
	}
	bool flag=pDb->Execute(SString::toFormat("delete from t_ssp_data_window_condition where dw_sn=%d and con_sn=%d",m_iNo,curRow+1));
	if (!flag)
	{
		QMessageBox::warning(this,tr("警告"),tr("数据库中删除条件失败"),QMessageBox::Ok);
		return;
	}
	flag=pDb->Execute(SString::toFormat("update t_ssp_data_window_condition set con_sn=con_sn-1 where dw_sn=%d and con_sn>%d",m_iNo,curRow+1));
	//update为0返回失败，不能return
	if(flag==false)
	{
		QMessageBox::warning(this,tr("警告"),tr("数据库中删除条件失败"),QMessageBox::Ok);
		return;
	}
	m_table->removeRow(curRow);
	//int irow = m_table->rowCount();
	m_table->blockSignals(true);
	for(int j=curRow;j < m_table->rowCount();j++)
	{
		QString strNewIndex = QString::number(m_table->item(j,0)->text().toInt()-1);
		m_table->item(j,0)->setText(strNewIndex);
	}
	m_table->blockSignals(false);
	if (m_table->rowCount() <= 0)
	{
		removeButton->setEnabled(true);
	}
}
bool view_dw_condition_edit::insertRow(int rowNum)
{
	if(m_iNo<0)
		return false;
	bool flag=pDb->Execute(SString::toFormat("insert into t_ssp_data_window_condition (dw_sn,con_sn,con_name,cmp_type,input_type,is_option,ref_name,def_value,dyn_where) values (%d,%d,'新条件',1,1,0,'','','')",m_iNo,rowNum+1));
	if(flag==false)
	{
		QMessageBox::warning(this,tr("警告"),tr("数据库中插入新条件失败"),QMessageBox::Ok);
		return false;
	}
	m_table->blockSignals(true);
	m_table->insertRow(rowNum);
	QTableWidgetItem *item=new QTableWidgetItem();//条件序号
	item->setText(QString::number(rowNum+1));
	item->setFlags(item->flags()&~Qt::ItemIsEditable);
	m_table->setItem(rowNum,0,item);

	item=new QTableWidgetItem();//条件名称
	item->setText(tr("新条件"));
	m_table->setItem(rowNum,1,item);

	item=new QTableWidgetItem();//条件类型
	item->setText(tr("无"));
	m_table->setItem(rowNum,2,item);

	item=new QTableWidgetItem();//输入类型
	item->setText(tr("任意字符串输入"));
	m_table->setItem(rowNum,3,item);

	item=new QTableWidgetItem();//是否可选
	item->setText(tr("否"));
	m_table->setItem(rowNum,4,item);

	item=new QTableWidgetItem();//引用名
	m_table->setItem(rowNum,5,item);

	item=new QTableWidgetItem();//缺省值
	item->setFlags(item->flags() & ~Qt::ItemIsEditable);
	m_table->setItem(rowNum,6,item);

	item=new QTableWidgetItem();//动态查询
	m_table->setItem(rowNum,7,item);
	m_table->blockSignals(false);
	return true;
}

bool view_dw_condition_edit::insertAppointRow(int rowNum)
{
	if(m_iNo<0)
		return false;
	//插入指定位置时，先更新数据库中原来记录的序号
	int irowNum = m_table->rowCount();
	m_table->blockSignals(true);
	for(int i = irowNum; i > rowNum; i -- )
	{
		SString sql = SString::toFormat("update t_ssp_data_window_condition set con_sn=%d where dw_sn=%d and con_sn=%d", i+1,m_iNo,i);
		bool flag=pDb->Execute(sql);
		if(flag==false)
		{
			QMessageBox::warning(this,tr("警告"),tr("数据库更新失败失败[%1]").arg(sql.data()),QMessageBox::Ok);
			m_table->blockSignals(false);
			return false;
		}
		m_table->item(i-1,0)->setText(QString::number(i+1));
	}

	bool flag=pDb->Execute(SString::toFormat("insert into t_ssp_data_window_condition (dw_sn,con_sn,con_name,cmp_type,input_type,is_option,ref_name,def_value,dyn_where) values (%d,%d,'新条件',1,1,0,'','','')",m_iNo,rowNum+1));
	if(flag==false)
	{
		QMessageBox::warning(this,tr("警告"),tr("数据库中插入新条件失败"),QMessageBox::Ok);
		m_table->blockSignals(false);
		return false;
	}

	m_table->insertRow(rowNum);
	QTableWidgetItem *item=new QTableWidgetItem();//条件序号
	item->setText(QString::number(rowNum+1));
	item->setFlags(item->flags()&~Qt::ItemIsEditable);
	m_table->setItem(rowNum,0,item);

	item=new QTableWidgetItem();//条件名称
	item->setText(tr("新条件"));
	m_table->setItem(rowNum,1,item);

	item=new QTableWidgetItem();//条件类型
	item->setText(tr("无"));
	m_table->setItem(rowNum,2,item);

	item=new QTableWidgetItem();//输入类型
	item->setText(tr("任意字符串输入"));
	m_table->setItem(rowNum,3,item);

	item=new QTableWidgetItem();//是否可选
	item->setText(tr("否"));
	m_table->setItem(rowNum,4,item);

	item=new QTableWidgetItem();//引用名
	m_table->setItem(rowNum,5,item);

	item=new QTableWidgetItem();//缺省值
	item->setFlags(item->flags()& ~Qt::ItemIsEditable);
	m_table->setItem(rowNum,6,item);

	item=new QTableWidgetItem();//动态查询
	m_table->setItem(rowNum,7,item);
	m_table->blockSignals(false);
	return true;
}
void view_dw_condition_edit::editItem(QTableWidgetItem *item)
{
	m_table->blockSignals(true);
	int row=item->row();
	int col=item->column();
	SRecordset conTable;
	if(col==1)
	{
		if(!pDb->Execute(SString::toFormat("update t_ssp_data_window_condition set con_name='%s' where dw_sn=%d and con_sn=%d",item->text().toStdString().data(),m_iNo,row+1)))
		{
			QMessageBox::warning(this,tr("警告"),tr("数据库中修改条件名称失败"),QMessageBox::Ok);
			m_table->blockSignals(false);
			return;
		}
	}
	else if(col==2)
	{
		int conType= m_cmpTypeMap.key(item->text()).toInt();
		if (conType == 0 || conType == 3 || conType==4)
		{
			m_table->item(row,5)->setFlags(m_table->item(row,5)->flags()| Qt::ItemIsEditable);
			
		} else {
			m_table->item(row,5)->setFlags(m_table->item(row,5)->flags()&~Qt::ItemIsEditable);
		}
		m_table->item(row,5)->setText("");
		if(!pDb->Execute(SString::toFormat("update t_ssp_data_window_condition set cmp_type=%d,ref_name='' where dw_sn=%d and con_sn=%d",conType,m_iNo,row+1)))
		{
			QMessageBox::warning(this,tr("警告"),tr("数据库中修改条件类型失败"),QMessageBox::Ok);
			m_table->blockSignals(false);
			return;
		}
	}
	else if(col==3)
	{
		int inputType = m_inputTypeMap.key(item->text()).toInt();
		if (inputType == 4 || inputType == 5)
		{
			m_table->item(row,6)->setFlags(m_table->item(row,6)->flags()| Qt::ItemIsEditable);

		} else {
			m_table->item(row,6)->setFlags(m_table->item(row,6)->flags()&~Qt::ItemIsEditable);
		}
		m_table->item(row,6)->setText("");
		if(!pDb->Execute(SString::toFormat("update t_ssp_data_window_condition set input_type=%d,def_value='' where dw_sn=%d and con_sn=%d",inputType,m_iNo,row+1)))
		{
			m_table->blockSignals(false);
			return;
		}
	}
	else if(col==4)
	{
		int isOpt=1;
		if(item->text()==tr("是"))
			isOpt=1;
		else if(item->text()==tr("否"))
			isOpt=0;
		if(!pDb->Execute(SString::toFormat("update t_ssp_data_window_condition set is_option=%d where dw_sn=%d and con_sn=%d",isOpt,m_iNo,row+1)))
		{
			m_table->blockSignals(false);
			return;
		}
	}
	else if(col==5)
	{
		SString tmpName= m_dataRefMap.key(item->text()).toStdString().c_str();
#if 0
		for(int j=0;j<m_refList.GetRows();j++)
		{
			if(m_refList.GetValue(j,4)==item->text().toStdString().data())
			{
				tmpName=m_refList.GetValue(j,2);
				break;
			}
		}
#endif
		if(!pDb->Execute(SString::toFormat("update t_ssp_data_window_condition set ref_name='%s' where dw_sn=%d and con_sn=%d",tmpName.data(),m_iNo,row+1)))
		{
			m_table->blockSignals(false);
			return;
		}
	}

	else if(col==6)
	{
		if(!pDb->Execute(SString::toFormat("update t_ssp_data_window_condition set def_value='%s' where dw_sn=%d and con_sn=%d",item->text().toStdString().data(),m_iNo,row+1)))
		{
			m_table->blockSignals(false);
			return;
		}
	}
	else if(col==7)
	{
		if(!pDb->Execute(SString::toFormat("update t_ssp_data_window_condition set dyn_where='%s' where dw_sn=%d and con_sn=%d",item->text().toStdString().data(),m_iNo,row+1)))
		{
			m_table->blockSignals(false);
			return;
		}
	}
	m_table->blockSignals(false);
	m_table->resizeColumnsToContents();
}
QTableWidget *view_dw_condition_edit::getTableData()
{
	return m_table;
}
void view_dw_condition_edit::slotClickSel(QTableWidgetItem *item,QTableWidgetItem *pre)
{
	S_UNUSED(pre);
	//S_UNUSED(item);
	if (item->row() >=0)
	{
		removeButton->setEnabled(true);
	} else {
		removeButton->setEnabled(false);
	}
}
//////////////////////////////////////////////////////////////////////

dwconditionDelegate::dwconditionDelegate(QObject *parent)
	: QStyledItemDelegate(parent)
{
	this->parent=static_cast<view_dw_condition_edit*>(parent);
}



QWidget* dwconditionDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	switch(index.column())
	{
	case 2:
		{
			QComboBox* editor = new QComboBox(parent);
			editor->insertItem(0,tr("无"));
			editor->insertItem(1,tr("字符比较"));
			editor->insertItem(2,tr("数值比较"));
			editor->insertItem(3,tr("字符集合"));
			editor->insertItem(4,tr("整数集合"));
			editor->insertItem(5,tr("日期比较"));
			editor->insertItem(6,tr("日期时间比较"));
			editor->insertItem(7,tr("世纪秒比较"));
			return editor;
		}
		break;
	case 3:
		{
			QComboBox* editor = new QComboBox(parent);
			editor->insertItem(0,tr("任意字符串输入"));
			editor->insertItem(1,tr("数值输入"));
			editor->insertItem(2,tr("整数输入"));
			editor->insertItem(3,tr("日期输入"));
			editor->insertItem(4,tr("日期时间输入"));
			editor->insertItem(5,tr("下拉框单选输入"));
			editor->insertItem(6,tr("列表框多选输入"));
			return editor;
		}
		break;
	case 4:
		{
			QComboBox* editor = new QComboBox(parent);
			editor->insertItem(0,tr("是"));
			editor->insertItem(1,tr("否"));
			return editor;
		}
		break;
	case 5:
		{
			QComboBox* editor = new QComboBox(parent);
			editor->addItem("");
			QString strCmptext = this->parent->getTableData()->item(index.row(),2)->text();
			if(strCmptext ==tr("无") ||
				strCmptext ==tr("字符集合") ||
				strCmptext ==tr("整数集合"))
			{
				SString sql;
				if (this->parent->m_strComText != "全部" && this->parent->m_bIsChecked)
				{
					sql = SString::toFormat("select dw_desc from t_ssp_data_window where dstype=1 and cls_name='%s' order by cls_name desc,dw_desc",
						this->parent->m_strComText.toStdString().c_str());
					
				} else {
					sql = SString::toFormat("select dw_desc from t_ssp_data_window where dstype=1 and dw_desc is not null order by cls_name desc,dw_desc",
						this->parent->m_strComText.toStdString().c_str());
				}
				SRecordset rs;
				int ret = DB->RetrieveRecordset(sql,rs);
				if (ret < 0)
				{
					LOGERROR("查询数据失败%s",sql.data());
				}
				for (int i = 0; i < rs.GetRows(); i ++)
				{
					editor->addItem(rs.GetValue(i,0).data());
				}
				
			}
			return editor;
		}
		break;
	case 6:
		{
			QComboBox* editor = new QComboBox(parent);
			editor->insertItem(0,tr(""));
			QString strText = this->parent->getTableData()->item(index.row(),3)->text();
			if(strText == tr("日期输入"))
			{
				editor->insertItem(1,tr("TODAY"));
				editor->insertItem(2,tr("YESTERDAY"));
				editor->insertItem(3,tr("LASTWEEK"));
				editor->insertItem(4,tr("LASTMONTH"));
				editor->insertItem(5,tr("LASTYEAR"));
			}
			if(strText == tr("日期时间输入"))
			{
				editor->insertItem(1,tr("TODAY_BEGIN"));
				editor->insertItem(2,tr("TODAY_END"));
				editor->insertItem(3,tr("TODAY_NOW"));
				editor->insertItem(4,tr("YESTERDAY_BEGIN"));
				editor->insertItem(5,tr("YESTERDAY_END"));
				editor->insertItem(6,tr("YESTERDAY_NOW"));
			}
			return editor;
		}
		break;
	}
	QWidget *widget=QStyledItemDelegate::createEditor(parent,option,index);
	return widget;
}

void dwconditionDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
	QComboBox* comboEditor = qobject_cast<QComboBox* >(editor);
	if (comboEditor)
	{		
		QString text = index.model()->data(index,Qt::EditRole).toString();
		connect(comboEditor,SIGNAL(currentIndexChanged(QString)),this,SLOT(comboItemChanged(QString)));
		int index1 = comboEditor->findText(text);	

		comboEditor->setCurrentIndex(index1);	
	}

	QStyledItemDelegate::setEditorData(editor,index);

}
void dwconditionDelegate::comboItemChanged(QString text)
{
	parent->getTableData()->currentItem()->setText(text);
}
void dwconditionDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
	QStyledItemDelegate::setModelData(editor,model,index);
}