#include "panel_sim_param_set.h"
#include "view_home.h"
#include "GuiApplication.h"
////////////////////  CSmvOutCtrlDelegate  /////////////////////////
CSmvOutCtrlDelegate::CSmvOutCtrlDelegate(QObject *parent, stuSclVtIedDatasetFcda* pCur)
	: QItemDelegate(parent)
{
	pCurFcda = pCur;
}

CSmvOutCtrlDelegate::~CSmvOutCtrlDelegate()
{

}

QWidget * CSmvOutCtrlDelegate::createEditor(QWidget * parent,const QStyleOptionViewItem & option,const QModelIndex & index) const
{
	switch(index.column())
	{
		case 1:
		{
			QLineEdit * lineEdit = new QLineEdit(parent);		
			int iPrimaryRate = floor(pCurFcda->primary_rated);
			int iSecondaryRate = floor(pCurFcda->secondary_rated);
			int iMin = 0;//iPrimaryRate>iSecondaryRate ? iSecondaryRate:iPrimaryRate;
			int iMax = 99999999;//iPrimaryRate>iSecondaryRate ? iPrimaryRate:iSecondaryRate;
			QValidator *validatorPort=new QIntValidator(iMin,iMax, lineEdit);
			lineEdit->setValidator(validatorPort);
			//lineEdit->setToolTip(tr("有效值范围%1-%2").arg(iMin).arg(iMax));
			return lineEdit;
			break;
		}
		case 2:
			{//\b0[xX][0-9a-fA-F]{0,4}\b
				QRegExp rx("0[xX][0-9a-fA-F]{0,4}");
				QLineEdit * lineEdit = new QLineEdit(parent);
				QValidator *validatorPort=new QRegExpValidator(rx, lineEdit);
				lineEdit->setValidator(validatorPort);
				return lineEdit;
				break;
			}
		case 4:
		case 5:
		{
			QLineEdit * lineEdit = new QLineEdit(parent);
			bool bOk;
			float fMin = 0;
			float fMax = 999999999;
			QDoubleValidator *validatorPort=new QDoubleValidator(fMin,fMax,3, lineEdit);
			validatorPort->setNotation(QDoubleValidator::StandardNotation);
			lineEdit->setValidator(validatorPort);
			return lineEdit;
			break;
		}
		default:
		{
			QLineEdit * lineEdit = new QLineEdit(parent);
			return lineEdit;
			break;
		}
	}
	return NULL;
}

void CSmvOutCtrlDelegate::updateEditorGeometry(QWidget *editor,const QStyleOptionViewItem &option,const QModelIndex &) const
{
	if(editor)
	{
		editor->setGeometry(option.rect);
	}
}

void CSmvOutCtrlDelegate::setEditorData(QWidget *editor,const QModelIndex &index) const
{
	if(!index.isValid()) 
		return;

	QString text = index.data(Qt::DisplayRole).toString();

	QComboBox * combo = qobject_cast<QComboBox*>(editor);
	if(combo)
		combo->setCurrentIndex(combo->findText(text));

	QLineEdit * lineEdit = qobject_cast<QLineEdit*>(editor);
	if(lineEdit)
		lineEdit->setText(text);

	QDateTimeEdit * dtEdit = qobject_cast<QDateTimeEdit*>(editor);
	if(dtEdit)
		dtEdit->setDateTime(QDateTime::fromString(text,"yyyy-MM-dd"));
}

void CSmvOutCtrlDelegate::setModelData(QWidget *editor,QAbstractItemModel *model,const QModelIndex &index) const
{
	QString text = QString::null;
	QDateTimeEdit * dtEdit = qobject_cast<QDateTimeEdit*>(editor);
	if(dtEdit) 
		text = dtEdit->dateTime().toString("yyyy-MM-dd");

	QComboBox * combo = qobject_cast<QComboBox*>(editor);
	if(combo) 
		text = combo->itemText(combo->currentIndex());

	QLineEdit * lineEdit = qobject_cast<QLineEdit*>(editor);
	if(lineEdit)
		text = lineEdit->text();

	if(model) 
		model->setData(index,QVariant(text),Qt::DisplayRole);
}

////////////////////  CGooseOutCtrlDelegate  /////////////////////////
CGooseOutCtrlDelegate::CGooseOutCtrlDelegate(QObject *parent, stuSclVtIedDatasetFcda* pCur)
	: QItemDelegate(parent)
{
	pCurFcda = pCur;
}

CGooseOutCtrlDelegate::~CGooseOutCtrlDelegate()
{

}

QWidget * CGooseOutCtrlDelegate::createEditor(QWidget * parent,const QStyleOptionViewItem & option,const QModelIndex & index) const
{//0:未知  1:字符串  2：整数  3：浮点数  4：bitstring  5:utctime  6:bool  7:品质  8:Struct ...

	if (index.column() == 1)
	{
		if (pCurFcda->vtype == 2 || pCurFcda->vtype == 8)
		{
			QLineEdit * lineEdit = new QLineEdit(parent);
			bool bOk;
			int iMin = 0;
			int iMax = 99999999;
			QValidator *validatorPort=new QIntValidator(iMin,iMax, lineEdit);
			lineEdit->setValidator(validatorPort);
			return lineEdit;
		} else if (pCurFcda->vtype == 3 )
		{
			QLineEdit * lineEdit = new QLineEdit(parent);
			bool bOk;
			float fMin = 0;
			float fMax = 999999999;
			QDoubleValidator *validatorPort=new QDoubleValidator(fMin,fMax,3, lineEdit);
			validatorPort->setNotation(QDoubleValidator::StandardNotation);
			lineEdit->setValidator(validatorPort);
			return lineEdit;
		} else if (pCurFcda->vtype == 4)
		{
			QComboBox* combox = new QComboBox(parent);
			combox->addItem("0:无效0");
			combox->addItem("1:合");
			combox->addItem("2:分");
			combox->addItem("3:无效3");
			//combox->setCurrentIndex(0);
			return combox;
		} else if (pCurFcda->vtype == 5)
		{
#if 1
			QRegExp rx("^([0-9]{3}[1-9]|[0-9]{2}[1-9][0-9]{1}|[0-9]{1}[1-9][0-9]{2}|[1-9][0-9]{3})-(((0[13578]|1[02])-(0[1-9]|[12][0-9]|3[01]))|((0[469]|11)-(0[1-9]|[12][0-9]|30))|(02-(0[1-9]|[1][0-9]|2[0-8]))) ((1|0)[0-9]|2[0-3]):([0-5][0-9]):([0-5][0-9]) [0-9]{1,6}$");
			QLineEdit * lineEdit = new QLineEdit(parent);
			QValidator *validatorPort=new QRegExpValidator(rx, lineEdit);
			lineEdit->setValidator(validatorPort);
			return lineEdit;
#endif
#if 0
			QLineEdit* lineEdit = new QLineEdit(parent);
			lineEdit->setToolTip("时间格式：yyyy-MM-dd hh:mm:ss zzzuuu");
			return lineEdit;
#endif
		}
		else 
		{
			QLineEdit* lineEdit = new QLineEdit(parent);
			return lineEdit;
		}
	}
	
	return NULL;
}

void CGooseOutCtrlDelegate::updateEditorGeometry(QWidget *editor,const QStyleOptionViewItem &option,const QModelIndex &) const
{
	if(editor)
	{
		editor->setGeometry(option.rect);
	}
}

void CGooseOutCtrlDelegate::setEditorData(QWidget *editor,const QModelIndex &index) const
{
	if(!index.isValid()) 
		return;

	QString text = index.data(Qt::DisplayRole).toString();

	QComboBox * combo = qobject_cast<QComboBox*>(editor);
	if(combo)
		combo->setCurrentIndex(combo->findText(text));

	QLineEdit * lineEdit = qobject_cast<QLineEdit*>(editor);
	if(lineEdit)
		lineEdit->setText(text);

	QDateTimeEdit * dtEdit = qobject_cast<QDateTimeEdit*>(editor);
	if(dtEdit)
		dtEdit->setDateTime(QDateTime::fromString(text,"yyyy-MM-dd"));
}

void CGooseOutCtrlDelegate::setModelData(QWidget *editor,QAbstractItemModel *model,const QModelIndex &index) const
{
	QString text = QString::null;
	QDateTimeEdit * dtEdit = qobject_cast<QDateTimeEdit*>(editor);
	if(dtEdit) 
		text = dtEdit->dateTime().toString("yyyy-MM-dd");

	QComboBox * combo = qobject_cast<QComboBox*>(editor);
	if(combo) 
		text = combo->itemText(combo->currentIndex());

	QLineEdit * lineEdit = qobject_cast<QLineEdit*>(editor);
	if(lineEdit)
		text = lineEdit->text();

	if(model) 
		model->setData(index,QVariant(text),Qt::DisplayRole);
}

QWidget * CGooseOutCtrlDelegate::setIntType(QWidget * parent, const QStyleOptionViewItem & option, const QModelIndex & index) const
{
	switch(index.column())
	{
	case 1:
		{
			QLineEdit * lineEdit = new QLineEdit(parent);
			bool bOk;
			int iMin = 0;//pCurParaInfo->strMinValue.toInt(&bOk,10);
			int iMax = 99999999;//pCurParaInfo->strMaxValue.toInt(&bOk,10);
			QValidator *validatorPort=new QIntValidator(iMin,iMax, lineEdit);
			//QValidator *validatorPort=new QIntValidator(pCurParaInfo->strMinValue.toInt(),pCurParaInfo->strMaxValue.toInt(), lineEdit);
			lineEdit->setValidator(validatorPort);
			//lineEdit->setToolTip(tr("有效值范围%1-%2").arg(iMin).arg(iMax));
			return lineEdit;
			break;
		}
	case 3:
	case 4:
		{
			QLineEdit * lineEdit = new QLineEdit(parent);
			bool bOk;
			float fMin = 0;//pCurParaInfo->strMinValue.toFloat(&bOk);
			float fMax = 999999999;//pCurParaInfo->strMaxValue.toFloat(&bOk);
			QDoubleValidator *validatorPort=new QDoubleValidator(fMin,fMax,3, lineEdit);
			//QDoubleValidator *validatorPort=new QDoubleValidator(pCurParaInfo->strMinValue.toFloat(),pCurParaInfo->strMaxValue.toFloat(),pCurParaInfo->iFloatPrecision, lineEdit);
			validatorPort->setNotation(QDoubleValidator::StandardNotation);
			lineEdit->setValidator(validatorPort);
			return lineEdit;
			break;
		}
	default:
		{
			QLineEdit * lineEdit = new QLineEdit(parent);
			return lineEdit;
			break;
		}
	}
	return NULL;
}
QWidget * CGooseOutCtrlDelegate::setFloatType(QWidget * parent, const QStyleOptionViewItem & option, const QModelIndex & index) const
{
	switch(index.column())
	{
	case 1:
	case 3:
	case 4:
		{
			QLineEdit * lineEdit = new QLineEdit(parent);
			bool bOk;
			float fMin = 0;//pCurParaInfo->strMinValue.toFloat(&bOk);
			float fMax = 999999999;//pCurParaInfo->strMaxValue.toFloat(&bOk);
			QDoubleValidator *validatorPort=new QDoubleValidator(fMin,fMax,3, lineEdit);
			//QDoubleValidator *validatorPort=new QDoubleValidator(pCurParaInfo->strMinValue.toFloat(),pCurParaInfo->strMaxValue.toFloat(),pCurParaInfo->iFloatPrecision, lineEdit);
			validatorPort->setNotation(QDoubleValidator::StandardNotation);
			lineEdit->setValidator(validatorPort);
			return lineEdit;
			break;
		}
	default:
		{
			QLineEdit * lineEdit = new QLineEdit(parent);
			return lineEdit;
			break;
		}
	}
	return NULL;
}

//////////////////////////////////
panel_sim_param_set::panel_sim_param_set(QWidget *parent)
	: CBaseView(parent)
{
	ui.setupUi(this);
	m_pCurTreeItem = NULL;
	m_pCurStuSclVtIedSmvOut = NULL;
	m_pCurStuSclVtIedGooseOut = NULL;
	m_bHasChanged = false;
	//initTable();
	CGuiApplication *pApp = (CGuiApplication*)SApplication::GetPtr();
	m_pSimConfig = &pApp->m_SimConfig;
	m_pSimManager = &pApp->m_SimManager;
	//onRefreshData();
	connect(ui.tableWidget,SIGNAL(itemChanged(QTableWidgetItem *)),this,SLOT(slotItemChanged(QTableWidgetItem *)));
}

panel_sim_param_set::~panel_sim_param_set()
{

}

void panel_sim_param_set::OnPreShow()
{
	onRefreshData();
}

void panel_sim_param_set::OnAftShow()
{
	onRefreshData();
}

void panel_sim_param_set::OnRefresh()
{
	m_bHasChanged = false;
	onRefreshData();
}
void panel_sim_param_set::onRefreshData()
{
	initTableData();
}
void panel_sim_param_set::initGooseTable()
{
	ui.tableWidget->clear();
	QStringList labels;
	ui.tableWidget->setColumnCount(3);
	labels << "名称" << "值"<<"值类型"; 
	ui.tableWidget->setHorizontalHeaderLabels(labels);
	ui.tableWidget->setColumnWidth(0,150);
	ui.tableWidget->setColumnWidth(1,150);
	ui.tableWidget->setColumnWidth(2,100);
	SQt::SetTableWidgetStyle(ui.tableWidget);
	//ui.tableWidget->resizeColumnsToContents();
	
}

void panel_sim_param_set::initSmvTable()
{
	ui.tableWidget->clear();
	QStringList labels;
	ui.tableWidget->setColumnCount(6);
	labels << "名称" << "值"<<"品质"<<"值类型"<<"一次额定"<<"二次额定" ; 
	ui.tableWidget->setHorizontalHeaderLabels(labels);
	ui.tableWidget->setColumnWidth(0,150);
	ui.tableWidget->setColumnWidth(1,150);
	ui.tableWidget->setColumnWidth(2,100);
	ui.tableWidget->setColumnWidth(3,100);
	ui.tableWidget->setColumnWidth(4,100);
	ui.tableWidget->setColumnWidth(5,100);

	SQt::SetTableWidgetStyle(ui.tableWidget);
	//ui.tableWidget->resizeColumnsToContents();
	//connect(ui.tableWidget,SIGNAL(itemChanged(QTableWidgetItem *)),this,SLOT(slotItemChanged(QTableWidgetItem *)));
}

void panel_sim_param_set::initTableData()
{
	if (m_pCurTreeItem == NULL)
	{
		QMessageBox::warning(this,"告警","导航树节点对象指针为空！");
		return ;
	}
	//获取信号所属的子网对象
	QTreeWidgetItem* pSubNetItem = m_pCurTreeItem->parent()->parent();
	if (pSubNetItem == NULL)
	{
		QMessageBox::warning(this,"告警","导航树子网节点对象指针为空！");
		return ;
	}
	QString qstrSubnet = pSubNetItem->data(0,Qt::UserRole+1).toString();
	m_pCurSubNet = (stuSimSubnetwork*)atol(qstrSubnet.toStdString().data());


	eSimTreeNodeType node_type = (eSimTreeNodeType)m_pCurTreeItem->data(0,Qt::UserRole).toInt();
	m_iSimTreeNodeType = node_type;
	QString qstr = m_pCurTreeItem->data(0,Qt::UserRole+1).toString();
	ui.tableWidget->blockSignals(true);
	switch(node_type)
	{
		case SIM_TREE_HOME:
			//return m_pPanelSimRoot;
			break;
		case SIM_TREE_SUBNET:
			break;
		case SIM_TREE_SMV:
			break;
		case SIM_TREE_SMV_ITEM:
			initSmvTable();
			m_pCurStuSclVtIedSmvOut = (stuSclVtIedSmvOut*)atol(qstr.toStdString().data());
			initSmvOutData();
			break;
		case SIM_TREE_GOOSE:
			break;
		case SIM_TREE_GOOSE_ITEM:
			initGooseTable();
			m_pCurStuSclVtIedGooseOut = (stuSclVtIedGooseOut*)atol(qstr.toStdString().data());
			initGooseOutData();
			break;
		case SIM_TREE_MMSSVR:
			//return m_pPanelSimMmsSvr;
			break;
		case SIM_TREE_MMSSVR_ITEM:
			break;
		case SIM_TREE_MMSCLIENT:
			break;
		case SIM_TREE_MMSCLIENT_ITEM:
			break;
		case SIM_TREE_NTPSVR:
			break;
		case SIM_TREE_NTPSVR_ITEM:
			break;
		case SIM_TREE_NTPCLIENT:
			break;
		case SIM_TREE_NTPCLIENT_ITEM:
			break;
		case SIM_TREE_TCPSVR:
			break;
		case SIM_TREE_TCPSVR_ITEM:
			break;
		case SIM_TREE_TCPCLIENT:
			break;
		case SIM_TREE_TCPCLIENT_ITEM:
			break;
		case SIM_TREE_UDPSEND:
			break;
		case SIM_TREE_UDPSEND_ITEM:
			break;
		case SIM_TREE_ICMPSEND:
			break;
		case SIM_TREE_ICMPSEND_ITEM:
			break;
		case SIM_TREE_ARPSEND:
			break;
		case SIM_TREE_ARPSEND_ITEM:
			break;
		case SIM_TREE_STORM:
			break;
		case SIM_TREE_STORM_ITEM:
			break; 
	}
	ui.tableWidget->blockSignals(false);
}

void panel_sim_param_set::initSmvOutData()
{
	if (m_pCurStuSclVtIedSmvOut == NULL)
	{
		QMessageBox::warning(this,"告警","SMV信号对象为空！");
		return;
	}
	if (m_pCurStuSclVtIedSmvOut->m_pDataset == NULL)
	{
		return;
	}
	//ui.tableWidget->blockSignals(true);
	ui.tableWidget->clearContents();
	ui.tableWidget->setRowCount(0);
	for(int i = 0; i < m_pCurStuSclVtIedSmvOut->m_pDataset->m_Fcda.count(); i ++)
	{
		stuSclVtIedDatasetFcda* pCur = m_pCurStuSclVtIedSmvOut->m_pDataset->m_Fcda.at(i);
		if (pCur == NULL)
		{
			continue;
		}
		addSmvOutRow(pCur,true);
	}
	//ui.tableWidget->blockSignals(false);
	//ui.tableWidget->resizeColumnsToContents();
}

void panel_sim_param_set::slotItemChanged(QTableWidgetItem *item)
{
	ui.tableWidget->blockSignals(true);
	switch(m_iSimTreeNodeType)
	{
		case SIM_TREE_HOME:
			break;
		case SIM_TREE_SUBNET:
			break;
		case SIM_TREE_SMV:
			break;
		case SIM_TREE_SMV_ITEM:	
			doSmvOutItemChanged(item);
			break;
		case SIM_TREE_GOOSE:
			break;
		case SIM_TREE_GOOSE_ITEM:
			doGooseOutItemChanged(item);
			break;
		case SIM_TREE_MMSSVR:
			//return m_pPanelSimMmsSvr;
			break;
		case SIM_TREE_MMSSVR_ITEM:
			break;
		case SIM_TREE_MMSCLIENT:
			break;
		case SIM_TREE_MMSCLIENT_ITEM:
			break;
		case SIM_TREE_NTPSVR:
			break;
		case SIM_TREE_NTPSVR_ITEM:
			break;
		case SIM_TREE_NTPCLIENT:
			break;
		case SIM_TREE_NTPCLIENT_ITEM:
			break;
		case SIM_TREE_TCPSVR:
			break;
		case SIM_TREE_TCPSVR_ITEM:
			break;
		case SIM_TREE_TCPCLIENT:
			break;
		case SIM_TREE_TCPCLIENT_ITEM:
			break;
		case SIM_TREE_UDPSEND:
			break;
		case SIM_TREE_UDPSEND_ITEM:
			break;
		case SIM_TREE_ICMPSEND:
			break;
		case SIM_TREE_ICMPSEND_ITEM:
			break;
		case SIM_TREE_ARPSEND:
			break;
		case SIM_TREE_ARPSEND_ITEM:
			break;
		case SIM_TREE_STORM:
			break;
		case SIM_TREE_STORM_ITEM:
			break; 
	}
	ui.tableWidget->blockSignals(false);
}


void panel_sim_param_set::initGooseOutData()
{
	if (m_pCurStuSclVtIedGooseOut == NULL)
	{
		QMessageBox::warning(this,"告警","GOOSEOUT信号对象为空！");
		return;
	}
	if (m_pCurStuSclVtIedGooseOut->m_pDataset == NULL)
	{
		return;
	}
	//ui.tableWidget->blockSignals(true);
	ui.tableWidget->clearContents();
	ui.tableWidget->setRowCount(0);
	for(int i = 0; i < m_pCurStuSclVtIedGooseOut->m_pDataset->m_Fcda.count(); i ++)
	{
		stuSclVtIedDatasetFcda* pCur = m_pCurStuSclVtIedGooseOut->m_pDataset->m_Fcda.at(i);
		if (pCur == NULL)
		{
			continue;
		}
		addGooseOutRow(pCur);
	}
	//ui.tableWidget->blockSignals(false);
}


void panel_sim_param_set::addGooseOutRow(stuSclVtIedDatasetFcda* pCur, bool bAddRated)
{
	QString strPath = pCur->path.data();
	QStringList strPathSplitList = strPath.split("$");
	int iTmpSize = strPathSplitList.size();
	int iTmpCount = strPathSplitList.count();
	QString strTmp = strPathSplitList.at(strPathSplitList.count() -1);
	if (strPathSplitList.count() > 1 &&  strTmp == "DelayTRtg")
	{
		return;
	}
	int iRowCount = ui.tableWidget->rowCount();
	ui.tableWidget->insertRow(iRowCount);
	if (pCur->vtype != 6)
	{
		ui.tableWidget->setItemDelegateForRow(iRowCount,new CGooseOutCtrlDelegate(this,pCur));
	}
	QTableWidgetItem* item = new QTableWidgetItem(pCur->doi_desc);
	item->setTextAlignment(Qt::AlignCenter);
	item->setFlags(item->flags()&~Qt::ItemIsEditable);
	item->setData(Qt::UserRole,QVariant(SString::toFormat("%ld",(unsigned long)pCur).data()));
	ui.tableWidget->setItem(iRowCount,0,item);

	//0:未知  1:字符串  2：整数  3：浮点数  4：bitstring  5:utctime  6:bool  7:品质  8:Struct ...
	if (pCur->vtype == 6)
	{
		addTableWidgetOperItem(iRowCount,pCur);
	} else if (pCur->vtype == 4)
	{
		QString strText;
		if (pCur->value == "" || pCur->value == "0")
		{
			strText = "0:无效0";
		} else if (pCur->value == "1")
		{
			strText = "1:合";
		} else if (pCur->value == "2")
		{
			strText = "2:分";
		} else if (pCur->value == "3")
		{
			strText = "3:无效3";
		}
		item = new QTableWidgetItem(strText);
		item->setTextAlignment(Qt::AlignCenter);
		ui.tableWidget->setItem(iRowCount,1,item);	
	}  else if (pCur->vtype == 5)
	{
		QString strText;
		if (pCur->value.isEmpty())
		{
			strText = "";
		} else 
		{
			SString t = SString::GetAttributeValue(pCur->value,"t");

			if(pCur->value.length() == 19)
				strText = t.data();
			else 
			{
				SString strUsec = SString::GetAttributeValue(pCur->value,"usec");
				strText = tr("%1 %2").arg(t.data()).arg(strUsec.data());
			}
		}
		
		item = new QTableWidgetItem(strText);
		item->setTextAlignment(Qt::AlignCenter);
		ui.tableWidget->setItem(iRowCount,1,item);
	}
	else 
	{
		item = new QTableWidgetItem(pCur->value.data());
		item->setTextAlignment(Qt::AlignCenter);
		ui.tableWidget->setItem(iRowCount,1,item);
	}

	item = new QTableWidgetItem(pCur->bType);
	item->setTextAlignment(Qt::AlignCenter);
	item->setFlags(item->flags()&~Qt::ItemIsEditable);
	ui.tableWidget->setItem(iRowCount,2,item);
}

void panel_sim_param_set::addSmvOutRow(stuSclVtIedDatasetFcda* pCur, bool bAddRated)
{
	QString strPath = pCur->path.data();
	QStringList strPathSplitList = strPath.split("$");
	int iTmpSize = strPathSplitList.size();
	int iTmpCount = strPathSplitList.count();
	QString strTmp = strPathSplitList.at(strPathSplitList.count() -1);
	if (strPathSplitList.count() > 1 &&  strTmp == "DelayTRtg")
	{
		return;
	}
	int iRowCount = ui.tableWidget->rowCount();
	ui.tableWidget->insertRow(iRowCount);
	ui.tableWidget->setItemDelegateForRow(iRowCount,new CSmvOutCtrlDelegate(this,pCur));

	QTableWidgetItem* item = new QTableWidgetItem(pCur->doi_desc);
	item->setTextAlignment(Qt::AlignCenter);
	item->setFlags(item->flags()&~Qt::ItemIsEditable);
	item->setData(Qt::UserRole,QVariant(SString::toFormat("%ld",(unsigned long)pCur).data()));
	ui.tableWidget->setItem(iRowCount,0,item);

	QString strValue,strQuality;
	if (pCur->value.isEmpty())
	{
		strValue = "";
		strQuality = "";
	} else {
		strValue =SString::GetAttributeValue(pCur->value,"val");
		strQuality =SString::GetAttributeValue(pCur->value,"q");
	}

	item = new QTableWidgetItem(strValue);
	item->setTextAlignment(Qt::AlignCenter);
	ui.tableWidget->setItem(iRowCount,1,item);

	item = new QTableWidgetItem(strQuality);
	item->setTextAlignment(Qt::AlignCenter);
	ui.tableWidget->setItem(iRowCount,2,item);

	item = new QTableWidgetItem(pCur->bType);
	item->setTextAlignment(Qt::AlignCenter);
	item->setFlags(item->flags()&~Qt::ItemIsEditable);
	ui.tableWidget->setItem(iRowCount,3,item);

	item = new QTableWidgetItem(QString::number(pCur->primary_rated,'f',3));
	item->setTextAlignment(Qt::AlignCenter);
	ui.tableWidget->setItem(iRowCount,4,item);

	item = new QTableWidgetItem(QString::number(pCur->secondary_rated,'f',3));
	item->setTextAlignment(Qt::AlignCenter);
	ui.tableWidget->setItem(iRowCount,5,item);
}
void panel_sim_param_set::addTableWidgetOperItem(int row,stuSclVtIedDatasetFcda* pCurFcda)
{
	QPushButton * doPushBtn = new QPushButton();  
	doPushBtn->setIcon(QIcon(":/sim_iec61850/Resources/KEY04.ICO"));
	doPushBtn->setIconSize(QSize(24, 24));
	doPushBtn->setFlat(true);
	QString strText;
	if (pCurFcda->value.isEmpty() || pCurFcda->value == "0")
	{
		strText = "False";
	} else 
	{
		strText = "True";
	}
	doPushBtn->setText(strText);
	ui.tableWidget->setCellWidget(row,1,doPushBtn);
	connect(doPushBtn,SIGNAL(clicked()),this,SLOT(slotDoGooseOutPushButtonClicked()));
}
void panel_sim_param_set::slotDoGooseOutPushButtonClicked()
{
	QObject * object = QObject::sender();
	if (!object)
		return;

	int row = getRowByObject(object);
	if (row != -1)
	{
		QString strText = ((QPushButton*)(ui.tableWidget->cellWidget(row,1)))->text();
		QString qstr = ui.tableWidget->item(row,0)->data(Qt::UserRole).toString();
		stuSclVtIedDatasetFcda* pCurFcda = (stuSclVtIedDatasetFcda*)atol(qstr.toStdString().data());
		if (strText == "False")
		{
			pCurFcda->value = "1";
			((QPushButton*)(ui.tableWidget->cellWidget(row,1)))->setText("True");
		} else 
		{
			pCurFcda->value = "0";
			((QPushButton*)(ui.tableWidget->cellWidget(row,1)))->setText("False");
		}
		doSendGooseOut(m_pCurStuSclVtIedGooseOut);
		//m_pSimConfig->Save();

	}
}

void panel_sim_param_set::doGooseOutItemChanged(QTableWidgetItem *item)
{
	if (item == NULL)
	{
		return;
	}
	int row = item->row();
	if (row != -1)
	{
		QString qstr = ui.tableWidget->item(row,0)->data(Qt::UserRole).toString();
		stuSclVtIedDatasetFcda* pCurFcda = (stuSclVtIedDatasetFcda*)atol(qstr.toStdString().data());
		if (pCurFcda->vtype == 5)
		{
			if (item->text().isEmpty())
			{
				pCurFcda->value = "";
			}
			else
			{
				QString strCurText = item->text().trimmed();
				QString strTime;
				QString strUsec;
				QString strValue;
				if (strCurText.length() > 19)
				{
					QStringList strList = strCurText.split(" ");
					if (strList.size() >=3)
					{
						strTime = strList.at(0) + " " + strList.at(1);
						strUsec = strList.at(2);
					} else 
					{
						QMessageBox::warning(this, "警告","时间格式不对，请检查：yyyy-MM-dd HH:mm:ss zzzuuu");
						return;
					}
				} else 
				{
					strTime = strCurText;
					strUsec = "";
				}
				strValue = tr("t=%1;usec=%2;").arg(strTime).arg(strUsec);
				pCurFcda->value = strValue.toStdString().data();
			}
		} else if (pCurFcda->vtype == 4)
		{
				QString strText = item->text();
				QStringList strList = strText.split(":");
				QString strIndex = strList.size() > 1 ? strList.at(0):"0";
				pCurFcda->value = strIndex.toStdString().data();
		} else 
		{
			pCurFcda->value = item->text().toStdString().data();
		}
		
		doSendGooseOut(m_pCurStuSclVtIedGooseOut);
		//m_pSimConfig->Save();

	}
}

void panel_sim_param_set::doSmvOutItemChanged(QTableWidgetItem *item)
{
	if (item == NULL)
	{
		return;
	}
	int row = item->row();
	int column = item->column();

	if (row != -1 && column != -1)
	{
		QString qstr = ui.tableWidget->item(row,0)->data(Qt::UserRole).toString();
		stuSclVtIedDatasetFcda* pCurFcda = (stuSclVtIedDatasetFcda*)atol(qstr.toStdString().data());
		switch(column)
		{
			case 1:
			case 2:
			{
				QString strValue = ui.tableWidget->item(row,1)->text();
				QString strQuality = ui.tableWidget->item(row,2)->text();
				pCurFcda->value = SString::toFormat("val=%s;q=%s",strValue.toStdString().data(),strQuality.toStdString().data());
				break;
			}
			case 4:
			{
				pCurFcda->primary_rated = item->text().toFloat();
				break;
			}
			case 5:
			{
				pCurFcda->secondary_rated = item->text().toFloat();
				break;
			}
		}
		
		doSendSmvOut(m_pCurStuSclVtIedSmvOut);
		//m_pSimConfig->Save();

	}
}

void panel_sim_param_set::slotDoGooseOutComboxChanged(int index)
{
	QObject * object = QObject::sender();
	if (!object)
		return;

	int row = getRowByObject(object);
	if (row != -1)
	{
		int iIndex = ((QComboBox*)(ui.tableWidget->cellWidget(row,1)))->currentIndex();
		QString qstr = ui.tableWidget->item(row,0)->data(Qt::UserRole).toString();
		stuSclVtIedDatasetFcda* pCurFcda = (stuSclVtIedDatasetFcda*)atol(qstr.toStdString().data());
		pCurFcda->value = QString::number(iIndex).toStdString().data();
		doSendGooseOut(m_pCurStuSclVtIedGooseOut);
		//m_pSimConfig->Save();
	}
}

int	panel_sim_param_set::getRowByObject(QObject *object)
{
	if (!object)
		return -1;

	int row = -1;
	for (int i = 0; i < ui.tableWidget->rowCount(); i++)
	{
		QWidget * w = ui.tableWidget->cellWidget(i,1);
		if (w == object)
		{
			row = i;
			break;
		}
	}
	return row;
}
void panel_sim_param_set::doSendGooseOut(stuSclVtIedGooseOut* pCur)
{
	if (m_pCurSubNet == NULL ||
		m_pSimConfig == NULL ||
		m_pSimManager == NULL)
	{
		QMessageBox::warning(this,"警告","m_pCurSubNet，m_pSimConfig，m_pSimManager对象存在空指针对象！");
		return;
	}
	CGooseSimPort *pPort = m_pSimManager->m_GooseSimMGr.SearchPort(m_pCurSubNet->name);
	if (pCur->m_pDataset != NULL)
	{
		//if (!pPort->CheckGooseOutIsExist(pCur))
		{
			pPort->CheckGooseOutIsExist(pCur);
			CGooseSimulator *pGoose = new CGooseSimulator(pCur,m_pCurSubNet->mac);
			m_pSimManager->m_GooseSimMGr.AddGoose(m_pCurSubNet->name,pGoose);
		}	
	}
}

void panel_sim_param_set::doSendSmvOut(stuSclVtIedSmvOut* pCur)
{
	if (m_pCurSubNet == NULL ||
		m_pSimConfig == NULL ||
		m_pSimManager == NULL)
	{
		QMessageBox::warning(this,"警告","m_pCurSubNet，m_pSimConfig，m_pSimManager对象存在空指针对象！");
		return;
	}
	CMUSimPort *pPort = m_pSimManager->m_MuSimMgr.SearchPort(m_pCurSubNet->name);
	if (pCur->m_pDataset != NULL)
	{
		//if (!pPort->CheckGooseOutIsExist(pCur))
		{
			pPort->CheckSmvIsExist(pCur);
			CMUSimulator *pMu = new CMUSimulator(pCur,m_pCurSubNet->mac);
			m_pSimManager->m_MuSimMgr.AddMU(m_pCurSubNet->name,pMu);
		}	
	}
}
