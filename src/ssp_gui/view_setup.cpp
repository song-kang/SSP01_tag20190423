#include "view_setup.h"
#include "ssp_gui.h"
const int COLKEY=0;//关键字
const int COLDESC=1;//描述
const int COLCLASS=2;//类别
const int COLTYPE=3;//值类型
const int COLVALUE=4;//设定值
const int COLDIME=5;//量纲
const int COLFMTVALUE=6;//格式字符
enum SETUP_VAL_TYPE
{
	VAL_TYPE_BOOLE = 1,
	VAL_TYPE_ENUM,
	VAL_TYPE_INT,
	VAL_TYPE_FLOAT,
	VAL_TYPE_STRING,
	VAL_TYPE_REF,
};
view_setup::view_setup(QWidget *parent)
	: CBaseView(parent)
{
	ui.setupUi(this);
	pDb=DB;
	refresh();
}

view_setup::~view_setup()
{

}
void view_setup::refresh()
{
	while(ui.tabWidget->count()!=0)
	{
		setup_widget *delWidget=(setup_widget*)ui.tabWidget->widget(0);
		ui.tabWidget->removeTab(0);
		delWidget->deleteLater();
	}

	SRecordset tmpSet;
	pDb->RetrieveRecordset("select type_id,type_name from t_ssp_setup_type",tmpSet);
	for(int i=0;i<tmpSet.GetRows();i++)
	{
		int typeId=tmpSet.GetValue(i,0).toInt();
		QString text=tmpSet.GetValue(i,1).data();
		setup_widget *newWidget=new setup_widget(this,typeId);
		ui.tabWidget->addTab(newWidget,text);
	}
}
void view_setup::OnPreShow()
{
	SSP_GUI->ShowWaiting();
	ui.tabWidget->clear();
	refresh();
	SSP_GUI->HideWaiting();
}
/////////////////////////////////////////////////////////////////
setup_widget::setup_widget(QWidget *parent,int typeId)
	: CBaseView(parent)
{
	checkBox_class=new QCheckBox(this);
	checkBox_class->setText(tr("显示高级"));
	QVBoxLayout *layout1=new QVBoxLayout(this);
	QHBoxLayout *layoutBtn=new QHBoxLayout(this);
	QHBoxLayout *layoutTable=new QHBoxLayout(this);
	table=new QTableWidget(this);
	layoutBtn->addWidget(checkBox_class);
	layoutBtn->addStretch();
	layoutBtn->setMargin(4);
	layoutTable->addWidget(table);
	layout1->addLayout(layoutBtn);
	layout1->setMargin(4);
	layout1->addLayout(layoutTable);
	setLayout(layout1);
	this->typeId=typeId;
	pDb=DB;
	initTable();
	table->setItemDelegate(new setup_widgetDelegate(this));
	connect(checkBox_class,SIGNAL(stateChanged(int)),this,SLOT(on_checkBox_class(int)));
	connect(table,SIGNAL(itemChanged(QTableWidgetItem *)),this,SLOT(table_itemchanged(QTableWidgetItem *)));
}

setup_widget::~setup_widget()
{

}
void setup_widget::initTable()
{
	QStringList header;
	header<<tr("关键字")<<tr("描述")<<tr("类别")<<tr("值类型")<<tr("设定值")<<tr("量纲")<<tr("格式字符");
	table->clear();
	table->setColumnCount(7);
	table->setRowCount(0);
	table->setAlternatingRowColors(true);
	table->setHorizontalHeaderLabels(header);
	table->setColumnHidden(COLFMTVALUE,true);
	//table->setColumnHidden(COLTYPE,true);
	table->verticalHeader()->hide();
	table->setItemDelegate(new setup_widgetDelegate(this));
	table->setColumnWidth(COLKEY,100);
	table->setColumnWidth(COLDESC,200);
	table->setColumnWidth(COLCLASS,50);
	table->setColumnWidth(COLTYPE,100);
	table->setColumnWidth(COLVALUE,200);
	table->setColumnWidth(COLDIME,100);
	//table->horizontalHeader()->setStretchLastSection(true);
	SQt::SetTableWidgetStyle(table);
	refresh();
	
}
void setup_widget::refresh()
{
	table->blockSignals(true);
	SRecordset tmpSet;
	pDb->RetrieveRecordset(SString::toFormat("select set_key,set_desc,set_class,val_type,set_value,set_dime,val_fmt from t_ssp_setup where type_id=%d",typeId),tmpSet);
	for(int i=0;i<tmpSet.GetRows();i++)
	{
		if(checkBox_class->checkState()==Qt::Unchecked)
		{
			if(tmpSet.GetValue(i,2).toInt()==1)
				continue;
		}
		int row=table->rowCount();
		table->insertRow(row);

		QString setKey=tmpSet.GetValue(i,0).data();
		QTableWidgetItem *item=new QTableWidgetItem(setKey);
		item->setFlags(item->flags()&~Qt::ItemIsEditable);
		table->setItem(row,COLKEY,item);

		QString setDesc=tmpSet.GetValue(i,1).data();
		item=new QTableWidgetItem(setDesc);
		item->setFlags(item->flags()&~Qt::ItemIsEditable);
		table->setItem(row,COLDESC,item);

		QString setClass=classValueToStr(tmpSet.GetValue(i,2).toInt());
		item=new QTableWidgetItem(setClass);
		item->setFlags(item->flags()&~Qt::ItemIsEditable);
		table->setItem(row,COLCLASS,item);

		QString valType=typeValueToStr(tmpSet.GetValue(i,3).toInt());
		item=new QTableWidgetItem(valType);
		item->setFlags(item->flags()&~Qt::ItemIsEditable);
		table->setItem(row,COLTYPE,item);

		item=new QTableWidgetItem(tmpSet.GetValue(i,4).data());
		table->setItem(row,COLVALUE,item);

		QString setDime=tmpSet.GetValue(i,5).data();
		item=new QTableWidgetItem(setDime);
		item->setFlags(item->flags()&~Qt::ItemIsEditable);
		table->setItem(row,COLDIME,item);

		QString valFmt=tmpSet.GetValue(i,6).data();
		item=new QTableWidgetItem(valFmt);
		item->setFlags(item->flags()&~Qt::ItemIsEditable);
		table->setItem(row,COLFMTVALUE,item);
	}
	table->blockSignals(false);
	table->resizeColumnsToContents();
}
QString setup_widget::typeValueToStr(int value)
{
	switch(value)
	{
		case VAL_TYPE_BOOLE:
			return tr("布尔类型");
		case VAL_TYPE_ENUM:
			return tr("枚举类型");
		case  VAL_TYPE_INT:
			return tr("整数类型");
		case  VAL_TYPE_FLOAT:
			return tr("浮点类型");
		case VAL_TYPE_STRING:
			return tr("字符串类型");
		case VAL_TYPE_REF:
			return tr("引用类型");
		default:
			break;
	}
	return tr("字符串类型");
}
int setup_widget::typeStrToValue(QString str)
{
	if (str == tr("布尔类型"))
		return 1;
	if(str == tr("枚举类型"))
		return 2;
	else if(str==tr("整数类型"))
		return 3;
	else if(str==tr("浮点类型"))
		return 4;
	else if(str==tr("字符串类型"))
		return 5;
	else if (str == tr("引用类型"))
		return 6;
	return 1;
}
QString setup_widget::classValueToStr(int value)
{
	if(value==0)
		return tr("常规");
	else if(value==1)
		return tr("高级");
	return "常规";
}
void setup_widget::table_itemchanged(QTableWidgetItem *item)
{
	int col=item->column();
	int row=item->row();
	QString oldKey=table->item(row,0)->text();
	int type=typeStrToValue(table->item(row,COLTYPE)->text());
	QString valFmt=table->item(row,COLFMTVALUE)->text();
	if(col==COLVALUE)
	{
		QString itemTxt=item->text();
		if(type==VAL_TYPE_INT)//整形
		{
			if (!valFmt.isEmpty())
			{
				QStringList minMax = getMinMax(valFmt);
				if (itemTxt.toInt() < minMax.at(0).toInt() || itemTxt.toInt() > minMax.at(1).toInt())
				{
					QMessageBox::warning(this,tr("警告"),tr("设置值不合法，有效值范围【%1-%2】").arg(minMax.at(0)).arg(minMax.at(1)),QMessageBox::Ok);
					return;
				}
			}
		}
		if (type==VAL_TYPE_FLOAT)
		{
			if (!valFmt.isEmpty())
			{
				QStringList minMax = getMinMax(valFmt);
				if (itemTxt.toFloat() < minMax.at(0).toFloat() || itemTxt.toInt() > minMax.at(1).toFloat())
				{
					QMessageBox::warning(this,tr("警告"),tr("设置值不合法，有效值范围【%1-%2】").arg(minMax.at(0)).arg(minMax.at(1)),QMessageBox::Ok);
					return;
				}
			}	
		}
		if(type==VAL_TYPE_STRING)//minmax
		{
			if (!valFmt.isEmpty())
			{
				int lenMax=getLenMax(valFmt);
				if(itemTxt.size()>lenMax && lenMax != 0)
				{
					QMessageBox::warning(this,tr("警告"),tr("设置值字符长度超出范围，最大字符长度范围【%1】").arg(lenMax),QMessageBox::Ok);
					return;
				}
			}		
		}
		bool flag=pDb->Execute(SString::toFormat("update t_ssp_setup set set_value='%s' where set_key='%s'",\
			itemTxt.toStdString().data(),oldKey.toStdString().data()));
		if(flag==false)
		{
			QMessageBox::warning(this,tr("警告"),tr("在数据库更新值失败"),QMessageBox::Ok);
			return;
		}
		table->blockSignals(true);
		table->item(row,col)->setText(itemTxt);
		table->blockSignals(false);
	}
	table->resizeColumnsToContents();
}
void setup_widget::on_checkBox_class(int state)
{
	S_UNUSED(state);
	initTable();
}

QStringList setup_widget::getEnum(QString valFmt)
{
	//N1-描述1;N2-描述2;…
	QStringList tmpList;
	QStringList strList=valFmt.split(";");
	for(int i=0;i<strList.size();i++)
	{
		QString tmpStr=strList.at(i);
		if (tmpStr.isEmpty())
		{
			continue;
		}
		QStringList temMid = tmpStr.split("-");
		if (temMid.size() >= 2)
		{
			//int len = temMid.at(0).length();
			tmpStr.remove(0,temMid.at(0).length()+1);//去掉N1-...
			tmpList.append(tmpStr);
		}
		//tmpList.append(tmpStr);
	}
	return tmpList;
}
QStringList setup_widget::getMinMax(QString valFmt)
{
	//min=最小值;max=最大值;
	QStringList tmpList;
	if(valFmt.isEmpty())
	{
		return tmpList;
	}
	QStringList strList=valFmt.split(";");
	for(int i=0;i < strList.count();i++)
	{
		QString tmpStr=strList.at(i);
		if (tmpStr.isEmpty())
		{
			continue;
		}
		QStringList temMid = tmpStr.split("=");
		if (temMid.size() >= 2)
		{
			//int len = temMid.at(0).length();
			tmpStr.remove(0,temMid.at(0).length()+1);//去掉min=...
			tmpList.append(tmpStr);
		}
	}
	if (0< tmpList.size() && tmpList.size() < 2)
	{
		tmpList.append(tmpList.at(0));
	}
	return tmpList;
}

int setup_widget::getLenMax(QString valFmt)
{
	//maxlen=最大长度(0不限)
	//首先排除“；”解析影响
	QStringList tmpList;
	if(valFmt.isEmpty())
	{
		return 0;
	}
	QStringList strList = valFmt.split(";");
	if (strList.size() < 1)
	{
		return 0;
	}

	for(int i = 0; i < strList.size(); i++)
	{
		QString tmpStr=strList.at(i);
		if (tmpStr.isEmpty())
		{
			continue;
		}
		QStringList temMid = tmpStr.split("=");
		if (temMid.size() >= 2)
		{
			//int len = temMid.at(0).length();
			tmpStr.remove(0,temMid.at(0).length()+1);//去掉min=...
			tmpList.append(tmpStr);
		}
	}
	if(tmpList.size() < 1)
	{
		return 0;
	}
	return tmpList.at(0).toInt();
}
QTableWidget* setup_widget::getTable()
{
	return table;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
setup_widgetDelegate::setup_widgetDelegate(QObject *parent)
	: QStyledItemDelegate(parent)
{
	this->parent=static_cast<setup_widget*>(parent);
}

QWidget* setup_widgetDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	switch(index.column())
	{
	case 4:
		{
			//代理对设定值列根据值类型列进行区分处理
			QString strType = this->parent->getTable()->item(index.row(),COLTYPE)->text();
			if (strType == tr("布尔类型"))
			{
				QComboBox* editor = new QComboBox(parent);
				editor->addItem("false");
				editor->addItem("true");
				editor->setCurrentIndex(0);
				return editor;
			} else 	if (strType == tr("枚举类型"))
			{
				QString valFmt=this->parent->getTable()->item(index.row(),COLFMTVALUE)->text();
				QStringList tmpList=this->parent->getEnum(valFmt);
				QComboBox* editor = new QComboBox(parent);
				for(int i=0;i<tmpList.size();i++)
				{
					editor->insertItem(i,tmpList.at(i));
				}
				if(editor->count() <=0)
				{
					editor->addItem("");
					return editor;
				}	
				else {
					return editor;
				}
			} else if (strType == tr("引用类型"))
			{
				QString valFmt=this->parent->getTable()->item(index.row(),COLFMTVALUE)->text();
				QString strRefName;
				bool bFlag = false;
				QComboBox* refCom = new QComboBox(parent);
				if (valFmt.size() > 4)
				{
					strRefName = valFmt.remove(0,4);//去掉ref=
					SString sql = SString::toFormat("select dw_sn from t_ssp_data_window where name='%s'",strRefName.toStdString().c_str());
					SRecordset refcls;
					
					int ret = DB->RetrieveRecordset(sql,refcls);
					if (ret < 0)
					{
						QMessageBox::warning(this->parent,tr("警告"),tr("数据查询失败[%1]").arg(sql.data()),QMessageBox::Ok);
						bFlag = false;
					} else if (ret == 0)
					{
						bFlag = false;
					} else {
						SString strDwsn = refcls.GetValue(0,0);
						refcls.clear();
						sql = SString::toFormat("select row_id,row_name from t_ssp_data_window_row where dw_sn=%s", strDwsn.data());
						ret = DB->RetrieveRecordset(sql,refcls);
						if (ret < 0)
						{
							QMessageBox::warning(this->parent,tr("警告"),tr("数据查询失败[%1]").arg(sql.data()),QMessageBox::Ok);
							bFlag = false;
						} else if (ret == 0)
						{
							bFlag = false;
						} else
						{
							for (int i = 0; i < refcls.GetRows(); i++)
							{
								refCom->addItem(refcls.GetValue(i,0).data());
							}
							refCom->setCurrentIndex(0);
							bFlag = true;
						}
					}
					
				}
				if (!bFlag)
				{
					refCom->addItem("");
					refCom->setCurrentIndex(0);
					return refCom;
				} else {
					return refCom;
				}
				
			}	
		}
		break;
	}
	QWidget *widget=QStyledItemDelegate::createEditor(parent,option,index);
	return widget;
}

void setup_widgetDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
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
void setup_widgetDelegate::comboItemChanged(QString text)
{
	parent->getTable()->currentItem()->setText(text);
}
void setup_widgetDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
	QStyledItemDelegate::setModelData(editor,model,index);
}