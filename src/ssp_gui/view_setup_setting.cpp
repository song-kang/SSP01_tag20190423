#include "view_setup_setting.h"
const int COLKEY=0;//关键字
const int COLDESC=1;//描述
const int COLCLASS=2;//类别
const int COLTYPE=3;//值类型
const int COLVALUE=4;//设定值
const int COLDIME=5;//量纲
const int COLFMTVALUE=6;//格式字符
const int COLKEYBAK=7;//关键字
enum SETUP_VAL_TYPE
{
	VAL_TYPE_BOOLE = 1,
	VAL_TYPE_ENUM,
	VAL_TYPE_INT,
	VAL_TYPE_FLOAT,
	VAL_TYPE_STRING,
	VAL_TYPE_REF,
};

view_val_fmt_dlg::view_val_fmt_dlg(QWidget* parent, int type, QString strFmtValue)
{
	m_iValType = type;
	m_strOldFmtValue = strFmtValue;
	m_pParent = static_cast<setup_setting_widget*>(parent);
	pDb = DB;
	Qt::WindowFlags flags=Qt::Dialog;
	flags |=Qt::WindowCloseButtonHint;
	setWindowFlags(flags);
	setupUi(type);
	connect(m_okPushButton,SIGNAL(clicked()),this,SLOT(slotOnOkPushButtonClick()));
	connect(m_cancelPushButton,SIGNAL(clicked()),this,SLOT(reject()));
}
view_val_fmt_dlg::~view_val_fmt_dlg()
{

}
void view_val_fmt_dlg::setupUi(int type)
{
	switch(type)
	{
		case VAL_TYPE_ENUM:
			initEnumSet();
			break;
		case VAL_TYPE_INT:
		case VAL_TYPE_FLOAT:
			initIntOrFloatSet();
			break;
		case  VAL_TYPE_STRING:
			initStringSet();
			break;
		case VAL_TYPE_REF:
			initRefSet();
			break;
		default:
			break;
	}
}
//void view_val_fmt_dlg::contextMenuEvent(QContextMenuEvent * event)
//{
//	if (m_iValType != VAL_TYPE_ENUM)
//	{
//		return;
//	}
//	m_enumSubMenu->exec(QCursor::pos());
//}
void view_val_fmt_dlg::slotcustomContextMenuRequested(const QPoint& pos)
{
	S_UNUSED(pos);
	m_enumSubMenu->exec(QCursor::pos());
}
void view_val_fmt_dlg::slotDelOneEnum()
{
	int iRow = m_enumTableWidget->currentRow();  
	if( iRow < 0 )  
	{
		QMessageBox::warning(this,tr("警告"),tr("没有选择对象，请先选择要删除的行对象！"),QMessageBox::Ok);
		return;  
	}
	QString strMsg = "确定要删除当前第" + QString::number(iRow) + "行吗？";
	int ch = QMessageBox::warning(NULL, "Warning",  
		strMsg,  
		QMessageBox::Yes | QMessageBox::No,  
		QMessageBox::No);  

	if ( ch != QMessageBox::Yes )  
		return; 
	m_enumTableWidget->removeRow(iRow);
}
void view_val_fmt_dlg::initEnumTableWidget()
{
	m_enumTableWidget =  new QTableWidget(this);
	QStringList header;
	header<<tr("编号")<<tr("名称")<<tr("indexBck");
	m_enumTableWidget->setColumnCount(3);
	m_enumTableWidget->setHorizontalHeaderLabels(header);
	m_enumTableWidget->setAlternatingRowColors(true);
	m_enumTableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
	m_enumTableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
	m_enumTableWidget->horizontalHeader()->setStretchLastSection(true);
	m_enumTableWidget->verticalHeader()->hide();
	m_enumTableWidget->setColumnWidth(0,60);
	m_enumTableWidget->setColumnWidth(1,100);
	m_enumTableWidget->setColumnWidth(2,100);
	m_enumTableWidget->setColumnHidden(2,true);//隐藏sn
	SQt::SetTableWidgetStyle(m_enumTableWidget);
	connect(m_enumTableWidget,SIGNAL(itemChanged(QTableWidgetItem *)),this,SLOT(slotTableItemchanged(QTableWidgetItem *)));
}
void view_val_fmt_dlg::initIntOrFloatSet()
{
	QLabel *minLabel = new QLabel(tr("最小值："));
	QLabel *maxLabel = new QLabel(tr("最大值："));
	m_minLineEdit = new QLineEdit;
	m_maxLineEdit = new QLineEdit;
	QGridLayout *gridLayout = new QGridLayout;
	gridLayout->addWidget(minLabel,0,0,1,1);
	gridLayout->addWidget(maxLabel,1,0,1,1);
	gridLayout->addWidget(m_minLineEdit,0,1,1,3);
	gridLayout->addWidget(m_maxLineEdit,1,1,1,3);
	m_okPushButton = new QPushButton(tr("确定(&O)"));

	m_cancelPushButton = new QPushButton(tr("取消(&C)"));
	QIcon iconOk;
	iconOk.addFile(QString::fromUtf8(":/ssp_gui/Resources/export2.gif"), QSize(), QIcon::Normal, QIcon::Off);
	m_okPushButton->setIcon(iconOk);
	m_okPushButton->setMaximumHeight(24);
	QIcon iconCancel;
	iconCancel.addFile(QString::fromUtf8(":/ssp_gui/Resources/del.gif"), QSize(), QIcon::Normal, QIcon::Off);
	m_cancelPushButton->setIcon(iconCancel);
	m_cancelPushButton->setMaximumHeight(24);
	QHBoxLayout *btnLayout = new QHBoxLayout;

	btnLayout->setSpacing(60);
	btnLayout->addWidget(m_okPushButton);
	btnLayout->addWidget(m_cancelPushButton);
	QVBoxLayout *dlgLayout = new QVBoxLayout;
	dlgLayout->setMargin(5);
	dlgLayout->addLayout(gridLayout);
	dlgLayout->addLayout(btnLayout);
	setLayout(dlgLayout);
	if (m_iValType == VAL_TYPE_FLOAT)
	{
		setWindowTitle(tr("浮点型设置"));
	} else {
		setWindowTitle(tr("整型设置"));
	}
	
	if (!m_strOldFmtValue.isEmpty())
	{
		QStringList strMinMaxList = m_pParent->getMinMax(m_strOldFmtValue);
		if (strMinMaxList.size() >1)
		{
			m_minLineEdit->setText(strMinMaxList.at(0));
			m_maxLineEdit->setText(strMinMaxList.at(1));
		}
	
	}
	resize(200,100); 
}
void view_val_fmt_dlg::initStringSet()
{
	QLabel *nameLabel = new QLabel(tr("字符串长度设置："));
	m_strlineEdit = new QLineEdit;
	m_strlineEdit->setToolTip("请输入大于等于0的正整数，0表示不限制");
	QHBoxLayout *hLayout = new QHBoxLayout;
	hLayout->setSpacing(5);
	hLayout->addWidget(nameLabel);
	hLayout->addWidget(m_strlineEdit);
	m_okPushButton = new QPushButton(tr("确定(&O)"));
	m_cancelPushButton = new QPushButton(tr("取消(&C)"));
	QIcon iconOk;
	iconOk.addFile(QString::fromUtf8(":/ssp_gui/Resources/export2.gif"), QSize(), QIcon::Normal, QIcon::Off);
	m_okPushButton->setIcon(iconOk);
	m_okPushButton->setMaximumHeight(24);
	QIcon iconCancel;
	iconCancel.addFile(QString::fromUtf8(":/ssp_gui/Resources/del.gif"), QSize(), QIcon::Normal, QIcon::Off);
	m_cancelPushButton->setIcon(iconCancel);
	m_cancelPushButton->setMaximumHeight(24);
	QHBoxLayout *btnLayout = new QHBoxLayout;

	btnLayout->setSpacing(60);

	btnLayout->addWidget(m_okPushButton);

	btnLayout->addWidget(m_cancelPushButton);

	QVBoxLayout *dlgLayout = new QVBoxLayout;

	dlgLayout->setMargin(5);

	dlgLayout->addLayout(hLayout);

	//dlgLayout->addStretch(20);

	dlgLayout->addLayout(btnLayout);

	setLayout(dlgLayout);

	setWindowTitle(tr("字符串长度设置"));
	if (!m_strOldFmtValue.isEmpty())
	{
		int iMaxLen = m_pParent->getLenMax(m_strOldFmtValue);
		m_strlineEdit->setText(QString::number(iMaxLen));
	}
	resize(200,70); 
}
void view_val_fmt_dlg::initEnumSet()
{
	initEnumTableWidget();
	QVBoxLayout* vmenuLayout = new QVBoxLayout;
	vmenuLayout->addWidget(m_enumTableWidget);
	m_addPushButton = new QPushButton("添加(&F)");
	QIcon iconAdd;
	iconAdd.addFile(QString::fromUtf8(":/ssp_gui/Resources/20070402165925169.gif"), QSize(), QIcon::Normal, QIcon::Off);
	m_addPushButton->setIcon(iconAdd);
	m_addPushButton->setMaximumHeight(24);
	connect(m_addPushButton,SIGNAL(clicked()),this,SLOT(slotAddNewRow()));

	m_delPushButton = new QPushButton(tr("删除(&D)"));
	QIcon iconDel;
	iconDel.addFile(QString::fromUtf8(":/ssp_gui/Resources/del.gif"), QSize(), QIcon::Normal, QIcon::Off);
	m_delPushButton->setIcon(iconDel);
	m_delPushButton->setMaximumHeight(24);
	connect(m_delPushButton,SIGNAL(clicked()),this, SLOT(slotDelOneEnum()));

	m_okPushButton = new QPushButton(tr("确定(&O)"));
	m_okPushButton->setMaximumHeight(24);
	m_cancelPushButton = new QPushButton(tr("取消(&C)"));
	m_cancelPushButton->setMaximumHeight(24);

	QIcon iconOk;
	iconOk.addFile(QString::fromUtf8(":/ssp_gui/Resources/export2.gif"), QSize(), QIcon::Normal, QIcon::Off);
	m_okPushButton->setIcon(iconOk);
	QIcon iconCancel;
	iconCancel.addFile(QString::fromUtf8(":/ssp_gui/Resources/del.gif"), QSize(), QIcon::Normal, QIcon::Off);
	m_cancelPushButton->setIcon(iconCancel);

	QHBoxLayout *btnLayout = new QHBoxLayout;
	btnLayout->setSpacing(5);
	btnLayout->addWidget(m_addPushButton);
	btnLayout->addWidget(m_delPushButton);
	btnLayout->addWidget(m_okPushButton);
	btnLayout->addWidget(m_cancelPushButton);

	QVBoxLayout *dlgLayout = new QVBoxLayout;
	dlgLayout->setMargin(5);
	dlgLayout->addLayout(vmenuLayout);
	dlgLayout->addLayout(btnLayout);
	setLayout(dlgLayout);
	setWindowTitle(tr("枚举设置"));

	if (!m_strOldFmtValue.isEmpty())
	{
		QStringList strEnumList = m_pParent->getEnumList(m_strOldFmtValue);
		for(int i = 0; i < strEnumList.size(); i++)
		{
			QStringList temMid = strEnumList.at(i).split("-");
			if (temMid.size() >= 2)
			{
				insertOneRowToEnumTableWidget(i,temMid.at(0),temMid.at(1));
			}
		}
	}
	resize(200,250); 
}

void view_val_fmt_dlg::insertOneRowToEnumTableWidget(int row, QString strIndex, QString strValue)
{
	m_enumTableWidget->blockSignals(true);
	m_enumTableWidget->insertRow(row);
	QTableWidgetItem *item=new QTableWidgetItem(strIndex);
	m_enumTableWidget->setItem(row,0,item);
	item=new QTableWidgetItem(strValue);
	m_enumTableWidget->setItem(row,1,item);
	item=new QTableWidgetItem(strIndex);
	m_enumTableWidget->setItem(row,2,item);
	m_enumTableWidget->blockSignals(false);
}
void view_val_fmt_dlg::initRefSet()
{
	QLabel *nameLabel = new QLabel(tr("引用集选择："));
	m_refComboBox = new QComboBox();
	m_refComboBox->setMinimumWidth(100);

	QHBoxLayout *hLayout = new QHBoxLayout;
	hLayout->setSpacing(5);
	hLayout->addWidget(nameLabel);
	hLayout->addWidget(m_refComboBox);

	m_okPushButton = new QPushButton(tr("确定(&O)"));

	m_cancelPushButton = new QPushButton(tr("取消(&C)"));
	QIcon iconOk;
	iconOk.addFile(QString::fromUtf8(":/ssp_gui/Resources/export2.gif"), QSize(), QIcon::Normal, QIcon::Off);
	m_okPushButton->setIcon(iconOk);
	m_okPushButton->setMaximumHeight(24);
	QIcon iconCancel;
	iconCancel.addFile(QString::fromUtf8(":/ssp_gui/Resources/del.gif"), QSize(), QIcon::Normal, QIcon::Off);
	m_cancelPushButton->setIcon(iconCancel);
	m_cancelPushButton->setMaximumHeight(24);
	QHBoxLayout *btnLayout = new QHBoxLayout;

	btnLayout->setSpacing(90);

	btnLayout->addWidget(m_okPushButton);

	btnLayout->addWidget(m_cancelPushButton);

	QVBoxLayout *dlgLayout = new QVBoxLayout;

	dlgLayout->setMargin(5);

	dlgLayout->addLayout(hLayout);

	//dlgLayout->addStretch();

	dlgLayout->addLayout(btnLayout);

	setLayout(dlgLayout);

	setWindowTitle(tr("引用集设置"));
	resize(230,60); 
	SString sql = "select name,dw_desc from t_ssp_data_window where dstype=1 and name is not null order by cls_name desc,dw_desc";
	SRecordset refcls;
	int ret = DB->RetrieveRecordset(sql,refcls);
	if (ret < 0)
	{
		QMessageBox::warning(this,tr("警告"),tr("数据查询失败[%1]").arg(sql.data()),QMessageBox::Ok);
		return;
	} 
	m_refMap.clear();
	m_refComboBox->clear();
	//m_refComboBox->addItem("");
	for (int i = 0; i < refcls.GetRows(); i++)
	{
		m_refComboBox->addItem(refcls.GetValue(i,1).data());
		m_refMap[refcls.GetValue(i,0).data()] = refcls.GetValue(i,1).data();
	}
	if (!m_strOldFmtValue.isEmpty())
	{
		QStringList strList = m_strOldFmtValue.split("=");
		if (strList.size() > 1)
		{
			QString strKey = strList.at(1);
			strKey = strKey.left(strKey.size()-1);//去掉;号
			QString showText = m_refMap[strKey];
			int index = m_refComboBox->findText(showText);	
			m_refComboBox->setCurrentIndex(index);	
		}
	}

}
void view_val_fmt_dlg::slotAddNewRow()
{
	int rowCount = m_enumTableWidget->rowCount();
	int newAddIndex = getMaxIndexFromEnumTableWidget() + 1;
	insertOneRowToEnumTableWidget(rowCount, QString::number(newAddIndex), "");
}

void view_val_fmt_dlg::slotTableItemchanged(QTableWidgetItem *item)
{
	if (item == NULL)
	{
		return;
	}
	int irow = item->row();
	int icol = item->column();
	if (icol == 0)//编号列
	{
		bool bOk = false;
		int iIndex = item->text().toInt(&bOk,10);
		S_UNUSED(iIndex);
		if (!bOk)
		{
			QMessageBox::warning(this,tr("警告"),tr("当前设置的编号非整数值，请重新设置！"),QMessageBox::Ok);
			m_enumTableWidget->item(irow,0)->setText(m_enumTableWidget->item(irow,2)->text());//
			return;
		}
		if (checkIndexIsExist(irow,item->text()))
		{
			QMessageBox::warning(this,tr("警告"),tr("编号重复，请重新设置！"),QMessageBox::Ok);
			m_enumTableWidget->item(irow,0)->setText(m_enumTableWidget->item(irow,2)->text());//
			return;
		} else
		{
			m_enumTableWidget->item(irow,2)->setText(item->text());//
		}
	}
}
int view_val_fmt_dlg::getMaxIndexFromEnumTableWidget()
{
	int iMax = 0;
	for (int i = 0; i < m_enumTableWidget->rowCount(); i++)
	{
		int iCurIndex = m_enumTableWidget->item(i,0)->text().toInt();
		if (iMax < iCurIndex)
		{
			iMax = iCurIndex;
		}
	}
	return iMax;
}
bool view_val_fmt_dlg::checkIndexIsExist(int irow, QString str)
{
	for (int i = 0; i < m_enumTableWidget->rowCount(); i ++)
	{
		if (i != irow)
		{
			QString strIndex = m_enumTableWidget->item(i,0)->text();
			if (strIndex != str)
			{
				continue;
			} else {
				 return true;
			}

		}
	}
	return false;
}
void view_val_fmt_dlg::slotOnOkPushButtonClick()
{
	switch(m_iValType)
	{
		case VAL_TYPE_BOOLE:
			break;
		case VAL_TYPE_ENUM:
		{
			int icount = m_enumTableWidget->rowCount();
			if (icount <= 0)
			{
				m_strShowInfo = "";
			}
			m_strShowInfo = "";
			for (int i = 0; i < icount; i++)
			{
				QString strCurIndex = m_enumTableWidget->item(i,0)->text();
				QString strCurText = m_enumTableWidget->item(i,1)->text();
				QString strOneEnumInfo = strCurIndex + "-" + strCurText + ";";
				m_strShowInfo += strOneEnumInfo;
			}
			break;
		}	
		case VAL_TYPE_INT:
		case VAL_TYPE_FLOAT:
			{
				QString strMin = m_minLineEdit->text();
				QString strMax = m_maxLineEdit->text();
				if (strMin.isEmpty() && strMin.isEmpty())
				{
					m_strShowInfo = "";
				} else 
				{
					if (strMin.isEmpty() || strMax.isEmpty())
					{
						QMessageBox::warning(this,"提示","最大最小可以不设，全部为空表示不限制，其中有个不为空，两个必须都要设置！",QMessageBox::Ok);
						return;
					} else {
						bool bminFlag = false;
						bool bmaxFlag = false;
						if (m_iValType == VAL_TYPE_INT)
						{
							int imin = strMin.toInt(&bminFlag,10);
							int imax = strMax.toInt(&bmaxFlag,10);
							if (!bmaxFlag & !bminFlag)
							{
								QMessageBox::warning(this,"提示","最大最小值必须为数值，请重新设置！",QMessageBox::Ok);
								return;
							}
							if (imin > imax)
							{
								QMessageBox::warning(this,"提示","最大值必须大于等于最小值！",QMessageBox::Ok);
								return;
							}
						}
						if (m_iValType == VAL_TYPE_FLOAT)
						{
							float fmin = strMin.toFloat(&bminFlag);
							float fmax = strMax.toFloat(&bmaxFlag);
							if (!bmaxFlag & !bminFlag)
							{
								QMessageBox::warning(this,"提示","最大最小值必须为数值，请重新设置！",QMessageBox::Ok);
								return;
							}
							if (fmin > fmax)
							{
								QMessageBox::warning(this,"提示","最大值必须大于等于最小值！",QMessageBox::Ok);
								return;
							}
						}
						m_strShowInfo = "min="+m_minLineEdit->text() + ";" + "max="+m_maxLineEdit->text()+";";
					}
				}
				break;
			}
		
		case VAL_TYPE_STRING:
		{
			QString strVal = m_strlineEdit->text();
			if (strVal.isEmpty())
			{
				m_strShowInfo = "";
			} else {
				bool bOk;
				int iVal = strVal.toInt(&bOk);
				if (!bOk)
				{
					QMessageBox::warning(this,"提示","字符长度必须为大于等于0的正整数，请重新设置！",QMessageBox::Ok);
					return;
				}
				if (iVal < 0)
				{
					QMessageBox::warning(this,"提示","字符长度必须大于等于0，0表示不限制！",QMessageBox::Ok);
					return;
				}
				m_strShowInfo = "maxlen="+strVal+";";
			}
			break;
		}
		case VAL_TYPE_REF:
		{
			QString strRef = m_refComboBox->currentText();
			if (strRef.isEmpty())
			{
				m_strShowInfo = "";
			} else {
				m_strShowInfo = "ref="+m_refMap.key(strRef)+";";
			}
			break;
		}	
		default:
			break;
	}
	QDialog::accept();
}

view_setup_setting::view_setup_setting(QWidget *parent)
	: CBaseView(parent)
{
	ui.setupUi(this);
	pDb=DB;
	refresh();
	connect(ui.pushButton_typeAdd,SIGNAL(clicked()),this,SLOT(on_pushButton_typeAdd()));
	connect(ui.pushButton_typeEdit,SIGNAL(clicked()),this,SLOT(on_pushButton_typeEdit()));
	connect(ui.pushButton_typeDel,SIGNAL(clicked()),this,SLOT(on_pushButton_typeDel()));
}

view_setup_setting::~view_setup_setting()
{

}
void view_setup_setting::refresh()
{
	while(ui.tabWidget->count()!=0)
	{
		setup_setting_widget *delWidget=(setup_setting_widget*)ui.tabWidget->widget(0);
		ui.tabWidget->removeTab(0);
		delWidget->deleteLater();
	}

	SRecordset tmpSet;
	pDb->RetrieveRecordset("select type_id,type_name from t_ssp_setup_type",tmpSet);
	for(int i=0;i<tmpSet.GetRows();i++)
	{
		int typeId=tmpSet.GetValue(i,0).toInt();
		QString text=tmpSet.GetValue(i,1).data();
		setup_setting_widget *newWidget=new setup_setting_widget(this,typeId);
		ui.tabWidget->addTab(newWidget,text);
	}
}
void view_setup_setting::on_pushButton_typeAdd()
{
	bool isOK; 
	QString text=QInputDialog::getText(this,tr("输入名称"),tr("请输入新类型名称"), QLineEdit::Normal, "", &isOK); 
	if(!isOK)
		return;	
	SRecordset tmpSet;
	pDb->RetrieveRecordset("select max(type_id) from t_ssp_setup_type",tmpSet);
	int typeId=tmpSet.GetValue(0,0).toInt()+1;

	bool flag=pDb->Execute(SString::toFormat("insert into t_ssp_setup_type (type_id,type_name) values (%d,'%s')",typeId,text.toStdString().data()));
	if(flag==false)
	{
		QMessageBox::warning(this,tr("警告"),tr("在数据库插入新类型失败"),QMessageBox::Ok);
		return;
	}
	setup_setting_widget *newWidget=new setup_setting_widget(this,typeId);
	ui.tabWidget->addTab(newWidget,text);
}
void view_setup_setting::on_pushButton_typeEdit()
{
	if(ui.tabWidget->currentIndex()<0)
		return;
	bool isOK; 
	QString text=QInputDialog::getText(this,tr("输入名称"),tr("请输入新类型名称"), QLineEdit::Normal, "", &isOK); 
	if(!isOK)
		return;	
	int typeId=((setup_setting_widget*)ui.tabWidget->currentWidget())->getTypeId();
	bool flag=pDb->Execute(SString::toFormat("update t_ssp_setup_type set type_name='%s' where type_id=%d",\
		text.toStdString().data(),typeId));
	if(flag==false)
	{
		QMessageBox::warning(this,tr("警告"),tr("在数据库更新类型名称失败"),QMessageBox::Ok);
		return;
	}
	ui.tabWidget->setTabText(ui.tabWidget->currentIndex(),text);
}
void view_setup_setting::on_pushButton_typeDel()
{
	if(ui.tabWidget->currentIndex()<0)
		return;
	setup_setting_widget *delWidget=(setup_setting_widget*)ui.tabWidget->currentWidget();
	int typeId=delWidget->getTypeId();
	bool flag=pDb->Execute(SString::toFormat("delete from t_ssp_setup_type where type_id=%d",typeId));
	if(flag==false)
	{
		QMessageBox::warning(this,tr("警告"),tr("在数据库删除类型失败"),QMessageBox::Ok);
		return;
	}
	flag=pDb->Execute(SString::toFormat("delete from t_ssp_setup where type_id=%d",typeId));
	if(flag==false)
	{
		QMessageBox::warning(this,tr("警告"),tr("在数据库删除类型下的配置失败"),QMessageBox::Ok);
		return;
	}
	ui.tabWidget->removeTab(ui.tabWidget->currentIndex());
	delWidget->deleteLater();
}

/////////////////////////////////////////////////////////////////
setup_setting_widget::setup_setting_widget(QWidget *parent,int typeId)
	: CBaseView(parent)
{
	layout()->deleteLater();
	pushButton_add=new QPushButton(tr("添加"),this);
	QIcon icon;
	icon.addFile(QString::fromUtf8(":/ssp_gui/Resources/20070402165925169.gif"), QSize(), QIcon::Normal, QIcon::Off);
	pushButton_add->setIcon(icon);
	pushButton_add->setMaximumSize(QSize(16777215, 24));

	pushButton_del=new QPushButton(tr("删除"),this);
	QIcon icon2;
	icon2.addFile(QString::fromUtf8(":/ssp_gui/Resources/del.gif"), QSize(), QIcon::Normal, QIcon::Off);
	pushButton_del->setIcon(icon2);
	pushButton_del->setMaximumSize(QSize(16777215, 24));
	QVBoxLayout *layout1=new QVBoxLayout(this);
	QHBoxLayout *layoutBtn=new QHBoxLayout(this);
	QHBoxLayout *layoutTable=new QHBoxLayout(this);
	
 	layoutBtn->addWidget(pushButton_add);
 	layoutBtn->addWidget(pushButton_del);
 	layoutBtn->addStretch();
 	layoutBtn->setContentsMargins(4,4,4,4);
	table=new QTableWidget(this);
	initTable();
 	layoutTable->addWidget(table);
	layout1->addLayout(layoutBtn);
	layout1->addLayout(layoutTable);
 	layout1->setContentsMargins(4,4,4,4);
	setLayout(layout1);
	this->typeId=typeId;
	pDb=DB;
	refresh();
	connect(pushButton_add,SIGNAL(clicked()),this,SLOT(on_pushButton_add()));
	connect(pushButton_del,SIGNAL(clicked()),this,SLOT(on_pushButton_del()));
	connect(table,SIGNAL(cellDoubleClicked(int,int)),this,SLOT(slotOnCellDoubleCliced(int,int)));
}

setup_setting_widget::~setup_setting_widget()
{

}
void setup_setting_widget::initTable()
{
	QStringList header;
	header<<tr("关键字")<<tr("描述")<<tr("类别")<<tr("值类型")<<tr("设定值")<<tr("量纲")<<tr("格式字符(双击编辑)")<<tr("关键字备份");
	table->clear();
	table->setColumnCount(8);
	table->setAlternatingRowColors(true);
	table->setHorizontalHeaderLabels(header);
	table->setColumnHidden(COLKEYBAK,true);
	table->setItemDelegate(new setup_setting_widgetDelegate(this));
	table->verticalHeader()->hide();
	table->horizontalHeader()->setStretchLastSection(true);
	table->setColumnWidth(COLKEY,100);
	table->setColumnWidth(COLDESC,200);
	table->setColumnWidth(COLVALUE,200);
	table->setColumnWidth(COLDIME,100);
	table->setColumnWidth(COLCLASS,50);
	table->setColumnWidth(COLTYPE,100);
	SQt::SetTableWidgetStyle(table);
	connect(table,SIGNAL(itemChanged(QTableWidgetItem *)),this,SLOT(table_itemchanged(QTableWidgetItem *)));
}
void setup_setting_widget::refresh()
{
	table->blockSignals(true);
	table->clearContents();
	table->setRowCount(0);
	SRecordset tmpSet;
	pDb->RetrieveRecordset(SString::toFormat("select set_key,set_desc,set_class,val_type,set_value,set_dime,val_fmt from t_ssp_setup where type_id=%d",typeId),tmpSet);
	for(int i=0;i<tmpSet.GetRows();i++)
	{
		int row= i ;
		table->insertRow(row);

		QString setKey=tmpSet.GetValue(i,0).data();
		QTableWidgetItem *item=new QTableWidgetItem(setKey);
		table->setItem(row,COLKEY,item);

		QString setDesc=tmpSet.GetValue(i,1).data();
		item=new QTableWidgetItem(setDesc);
		table->setItem(row,COLDESC,item);

		QString setClass=classValueToStr(tmpSet.GetValue(i,2).toInt());
		item=new QTableWidgetItem(setClass);
		table->setItem(row,COLCLASS,item);

		QString valType=typeValueToStr(tmpSet.GetValue(i,3).toInt());
		item=new QTableWidgetItem(valType);
		table->setItem(row,COLTYPE,item);

		item=new QTableWidgetItem(tmpSet.GetValue(i,4).data());
		table->setItem(row,COLVALUE,item);

		QString setDime=tmpSet.GetValue(i,5).data();
		item=new QTableWidgetItem(setDime);
		table->setItem(row,COLDIME,item);

		QString valFmt=tmpSet.GetValue(i,6).data();
		item=new QTableWidgetItem(valFmt);
		item->setFlags(item->flags()&~Qt::ItemIsEditable);
		table->setItem(row,COLFMTVALUE,item);

		item=new QTableWidgetItem(setKey);
		item->setFlags(item->flags()&~Qt::ItemIsEditable);
		table->setItem(row,COLKEYBAK,item);
	}
	table->blockSignals(false);
	table->resizeColumnsToContents();
}
void setup_setting_widget::table_itemchanged(QTableWidgetItem *item)
{
	int col=item->column();
	int row=item->row();
	QString oldKey=table->item(row,COLKEYBAK)->text();
	if(col==COLKEY)
	{
		SRecordset tmpSet;
		SString sql = SString::toFormat("select set_key from t_ssp_setup where set_key='%s'",item->text().toStdString().c_str());
		int ret = pDb->RetrieveRecordset(sql,tmpSet);
		if (ret < 0)
		{
			QMessageBox::warning(this,tr("警告"),tr("数据库查询失败【%1】").arg(sql.data()),QMessageBox::Ok);
			return;
		} else if (ret > 0)
		{
			QMessageBox::warning(this,tr("警告"),tr("关键字重复,更新失败"),QMessageBox::Ok);
			return;
		}

		bool flag=pDb->Execute(SString::toFormat("update t_ssp_setup set set_key='%s' where set_key='%s'",\
			item->text().toStdString().data(),oldKey.toStdString().data()));
		if(flag==false)
		{
			QMessageBox::warning(this,tr("警告"),tr("在数据库更新关键字失败"),QMessageBox::Ok);
			return;
		}
		table->blockSignals(true);
		table->item(row,COLKEYBAK)->setText(item->text());
		table->blockSignals(false);
	}
	if(col==COLDESC)
	{
		bool flag=pDb->Execute(SString::toFormat("update t_ssp_setup set set_desc='%s' where set_key='%s'",\
			table->item(row,col)->text().toStdString().data(),oldKey.toStdString().data()));
		if(flag==false)
		{
			QMessageBox::warning(this,tr("警告"),tr("在数据库更新描述失败"),QMessageBox::Ok);
			return;
		}
	}
	if(col==COLVALUE)
	{
		int iType = typeStrToValue(table->item(row,COLTYPE)->text());
		QString valFmt=table->item(row,COLFMTVALUE)->text();
		QString itemTxt = item->text();
		if(iType == VAL_TYPE_INT)//整形
		{
			bool bOK;
			int iVaule = itemTxt.toInt(&bOK,10);
			S_UNUSED(iVaule);
			if (!bOK)
			{
				QMessageBox::warning(this,tr("警告"),tr("设定值不合法，必须为数值，请重新设置！"),QMessageBox::Ok);
				return;
			}
			if (!valFmt.isEmpty())
			{
				QStringList minMax = getMinMax(valFmt);
				int isize = minMax.size();
				if (isize >= 2 && (itemTxt.toInt() < minMax.at(0).toInt() || itemTxt.toInt() > minMax.at(1).toInt()))
				{
					QMessageBox::warning(this,tr("警告"),tr("设置值不合法，有效值范围【%1-%2】").arg(minMax.at(0)).arg(minMax.at(1)),QMessageBox::Ok);
					return;
				}
			}
		
		}
		if (iType==VAL_TYPE_FLOAT)
		{
			bool bFlag = false;
			float fValue = itemTxt.toFloat(&bFlag);
			if (!bFlag)
			{
				QMessageBox::warning(this,"提示","设定值不合法，必须为数值，请重新设置！",QMessageBox::Ok);
				return;
			}
			if (!valFmt.isEmpty())
			{
				QStringList minMax = getMinMax(valFmt);
				int isize = minMax.size();
				if (isize >= 2 && (fValue < minMax.at(0).toFloat() || fValue > minMax.at(1).toFloat()))
				{
					QMessageBox::warning(this,tr("警告"),tr("设置值不合法，有效值范围【%1-%2】").arg(minMax.at(0)).arg(minMax.at(1)),QMessageBox::Ok);
					return;
				}
			}
			
		}
		if(iType==VAL_TYPE_STRING)//minmax
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
	}
	if(col==COLDIME)
	{
		bool flag=pDb->Execute(SString::toFormat("update t_ssp_setup set set_dime='%s' where set_key='%s'",\
			table->item(row,col)->text().toStdString().data(),oldKey.toStdString().data()));
		if(flag==false)
		{
			QMessageBox::warning(this,tr("警告"),tr("在数据库更新量纲失败"),QMessageBox::Ok);
			return;
		}
	}
	if(col==COLCLASS)
	{
		bool flag=pDb->Execute(SString::toFormat("update t_ssp_setup set set_class=%d where set_key='%s'",\
			classStrToValue(table->item(row,col)->text()),oldKey.toStdString().data()));
		if(flag==false)
		{
			QMessageBox::warning(this,tr("警告"),tr("在数据库更新类别失败"),QMessageBox::Ok);
			return;
		}
	}
	if(col==COLTYPE)
	{
		bool flag=pDb->Execute(SString::toFormat("update t_ssp_setup set val_type=%d,set_value=' ',val_fmt=' ' where set_key='%s'",\
			typeStrToValue(table->item(row,col)->text()),oldKey.toStdString().data()));
		if(flag==false)
		{
			QMessageBox::warning(this,tr("警告"),tr("在数据库更新值类型失败"),QMessageBox::Ok);
			return;
		}
		table->blockSignals(true);
		table->item(row,COLVALUE)->setText("");
		table->item(row,COLFMTVALUE)->setText("");
		table->blockSignals(false);
	}
	if(col==COLFMTVALUE)
	{
		bool flag=pDb->Execute(SString::toFormat("update t_ssp_setup set val_fmt='%s',set_value=' ' where set_key='%s'",\
			table->item(row,col)->text().toStdString().data(),oldKey.toStdString().data()));
		if(flag==false)
		{
			QMessageBox::warning(this,tr("警告"),tr("在数据库更新格式字符失败"),QMessageBox::Ok);
			return;
		}
		table->blockSignals(true);
		table->item(row,COLVALUE)->setText("");
		table->blockSignals(false);
	}

	table->resizeColumnsToContents();
}
int setup_setting_widget::typeStrToValue(QString str)
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
int setup_setting_widget::classStrToValue(QString str)
{
	if(str==tr("常规"))
		return 0;
	else if(str==tr("高级"))
		return 1;
	return 0;
}
QString setup_setting_widget::typeValueToStr(int value)
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
QString setup_setting_widget::classValueToStr(int value)
{
	if(value==0)
		return tr("常规");
	else if(value==1)
		return tr("高级");
	return tr("常规");
}
int setup_setting_widget::getTypeId()
{
	return typeId;
}
void setup_setting_widget::on_pushButton_add()
{
	SRecordset tmpSet;
	bool isOK; 
	QString text=QInputDialog::getText(this,tr("输入关键字"),tr("请输入唯一的关键字"), QLineEdit::Normal, "", &isOK); 
	if(!isOK)
	{
		return;
	}
	//检查关键字是否有效
	SString sql = SString::toFormat("select set_key from t_ssp_setup where set_key='%s'",text.toStdString().data());
	int ret = pDb->RetrieveRecordset(sql,tmpSet);
	if (ret < 0)
	{
		QMessageBox::warning(this,tr("警告"),tr("数据查询失败[%1]").arg(sql.data()),QMessageBox::Ok);
		return;
	} else if (ret > 0)
	{
		QMessageBox::warning(this,tr("警告"),tr("关键字重复，请重新添加！"),QMessageBox::Ok);
		return;
	}

	bool flag=pDb->Execute(SString::toFormat("insert into t_ssp_setup (set_key,type_id,set_desc,set_default,set_value,set_dime,set_class,val_type,val_fmt) values ('%s',%d,'','','','',0,1,'')",text.toStdString().data(),typeId));
	if(flag==false)
	{
		QMessageBox::warning(this,tr("警告"),tr("在数据库插入新配置项失败"),QMessageBox::Ok);
		return;
	}
	table->blockSignals(true);
	int curRow=table->rowCount();

	table->insertRow(curRow);
	QTableWidgetItem *item=new QTableWidgetItem(text);
	table->setItem(curRow,COLKEY,item);

	item=new QTableWidgetItem();
	table->setItem(curRow,COLDESC,item);

	item=new QTableWidgetItem("");
	table->setItem(curRow,COLVALUE,item);

	item=new QTableWidgetItem();
	table->setItem(curRow,COLDIME,item);

	item=new QTableWidgetItem(tr("常规"));
	table->setItem(curRow,COLCLASS,item);

	item=new QTableWidgetItem(tr("枚举类型"));
	table->setItem(curRow,COLTYPE,item);

	item=new QTableWidgetItem();
	table->setItem(curRow,COLFMTVALUE,item);

	item=new QTableWidgetItem(text);
	item->setFlags(item->flags()&~Qt::ItemIsEditable);
	table->setItem(curRow,COLKEYBAK,item);
	table->blockSignals(false);
	table->resizeColumnsToContents();
}
void setup_setting_widget::on_pushButton_del()
{
	int curRow=table->currentRow();
	if(curRow < 0)
	{
		QMessageBox::warning(this,tr("警告"),tr("无选择行，请先选中删除行"),QMessageBox::Ok);
		return;
		curRow=0;
	}
	int i_ret = QMessageBox::warning(this,tr("警告"),tr("是否删除第")+QString::number(curRow+1)+tr("行？"),QMessageBox::Yes,QMessageBox::No);
	if (i_ret==QMessageBox::No)
	{
		return;
	}
	SString sn=table->item(curRow,COLKEY)->text().toStdString().data();
	bool flag=pDb->Execute(SString::toFormat("delete from t_ssp_setup where set_key='%s'",sn.data()));
	if(flag==false)
	{
		QMessageBox::warning(this,tr("警告"),tr("数据库删除配置失败"),QMessageBox::Ok);
		return;
	}
	table->blockSignals(true);
	table->removeRow(curRow);
	table->blockSignals(false);

}
QStringList setup_setting_widget::getEnumList(QString valFmt)
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
		tmpList.append(tmpStr);
	}
	return tmpList;
}
QStringList setup_setting_widget::getEnum(QString valFmt)
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
	}
	return tmpList;
}
QStringList setup_setting_widget::getMinMax(QString valFmt)
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

int setup_setting_widget::getLenMax(QString valFmt)
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
QTableWidget* setup_setting_widget::getTable()
{
	return table;
}
void setup_setting_widget::slotOnCellDoubleCliced(int row, int column)
{
	if (column == COLFMTVALUE)
	{
		QString strType = table->item(row,COLTYPE)->text();
		int iType = typeStrToValue(strType);
		if (iType == VAL_TYPE_BOOLE)
		{
			return;
		}
		QString strFmtValue = table->item(row,COLFMTVALUE)->text();
		view_val_fmt_dlg dlg(this,iType,strFmtValue);
		if (dlg.exec() == QDialog::Accepted)
		{
			table->item(row,COLFMTVALUE)->setText(dlg.m_strShowInfo);
		}
	}
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
setup_setting_widgetDelegate::setup_setting_widgetDelegate(QObject *parent)
	: QStyledItemDelegate(parent)
{
	this->parent=static_cast<setup_setting_widget*>(parent);
}

QWidget* setup_setting_widgetDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	switch(index.column())
	{
		case COLCLASS:
			{
				QComboBox* editor = new QComboBox(parent);
				editor->insertItem(0,"常规");
				editor->insertItem(1,"高级");
				return editor;
				break;
			}
		case COLTYPE:
			{
				QComboBox* editor = new QComboBox(parent);
				editor->insertItem(0,"布尔类型");
				editor->insertItem(1,"枚举类型");
				editor->insertItem(2,"整数类型");
				editor->insertItem(3,"浮点类型");
				editor->insertItem(4,"字符串类型");
				editor->insertItem(5,"引用类型");
				return editor;
				break;
			}
		case COLVALUE:
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
				return editor;
			} else if (strType == tr("引用类型"))
			{
				QStringList strRefList = this->parent->getTable()->item(index.row(),COLFMTVALUE)->text().split(";");
				QString valFmt;
				if (strRefList.size() < 1)
				{
					valFmt = "";
				} else {
					valFmt = strRefList.at(0);
				}
				QString strRefName;
				bool bFlag = false;
				QComboBox* refCom = new QComboBox(parent);
				if (valFmt.size() > 4)
				{
					strRefName = valFmt.remove(0,4);//去掉ref=
					SString sql = SString::toFormat("select dw_sn,dbtype,dw_desc,dw_select from t_ssp_data_window where name='%s' and dstype=1",strRefName.toStdString().c_str());
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
				
						if (refcls.GetValue(0,1) == "1")
						{
							sql = SString::toFormat("select row_id,row_name from t_ssp_data_window_row where dw_sn=%s", strDwsn.data());
						} else {
							sql = refcls.GetValue(0,3);
						}
						refcls.clear();
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
			break;
		}	
	}
	QWidget *widget=QStyledItemDelegate::createEditor(parent,option,index);
	return widget;
}

void setup_setting_widgetDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
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
void setup_setting_widgetDelegate::comboItemChanged(QString text)
{
	parent->getTable()->currentItem()->setText(text);
}
void setup_setting_widgetDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
	QStyledItemDelegate::setModelData(editor,model,index);
}