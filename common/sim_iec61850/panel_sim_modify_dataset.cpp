#include "panel_sim_modify_dataset.h"
#include "view_home.h"
#include "GuiApplication.h"

////////////////////  CDataSetCtrlDelegate  /////////////////////////
CDataSetCtrlDelegate::CDataSetCtrlDelegate(QObject *parent,QTableWidget* pCur)
	: QItemDelegate(parent)
{
	m_pCurTableWidget = pCur;
}

CDataSetCtrlDelegate::~CDataSetCtrlDelegate()
{

}

QWidget * CDataSetCtrlDelegate::createEditor(QWidget * parent,const QStyleOptionViewItem & option,const QModelIndex & index) const
{
	switch(index.column())
	{
	case 0:
		{
			QLineEdit * lineEdit = new QLineEdit(parent);
			QIntValidator *validatorPort=new QIntValidator(1,65535,lineEdit);
			lineEdit->setValidator(validatorPort);
			return lineEdit;
		}
	case 1:
		{
			QComboBox* combox = new QComboBox(parent);
			combox->addItem("0:未知");
			combox->addItem("1:交流电压");
			combox->addItem("2:交流电流");
			combox->addItem("3:直流量");
			combox->addItem("4:额定延时时间");
			combox->setCurrentIndex(0);
			return combox;
		}
	case 2:
	case 3:
	case 4:
	case 5:
	case 6:
	case 9:
	case 10:
	case 11:
		{
			QString strAddr = m_pCurTableWidget->item(index.row(),0)->data(Qt::UserRole).toString();
			stuSclVtIedDatasetFcda* pCurFcda = (stuSclVtIedDatasetFcda*)strAddr.toLong();
			/*
			switch(pCurFcda->vtype)
			{
				case 0:
				case 1:
				{
					QLineEdit * lineEdit = new QLineEdit(parent);
					return lineEdit;
				}
				break;
				case 2:
				{
					QLineEdit * lineEdit = new QLineEdit(parent);		
					//int iPrimaryRate = floor(pCurFcda->primary_rated);
					//int iSecondaryRate = floor(pCurFcda->secondary_rated);
					int iMin = 0;//iPrimaryRate>iSecondaryRate ? iSecondaryRate:iPrimaryRate;
					int iMax = 999999;//iPrimaryRate>iSecondaryRate ? iPrimaryRate:iSecondaryRate;
					QValidator *validatorPort=new QIntValidator(iMin,iMax, lineEdit);
					lineEdit->setValidator(validatorPort);
					//lineEdit->setToolTip(tr("有效值范围%1-%2").arg(iMin).arg(iMax));
					return lineEdit;
				}
				break;
				case 3:
				{
					QLineEdit * lineEdit = new QLineEdit(parent);
					bool bOk;
					float fMin = 0;
					float fMax = 999999999;
					QDoubleValidator *validatorPort=new QDoubleValidator(fMin,fMax,3, lineEdit);
					validatorPort->setNotation(QDoubleValidator::StandardNotation);
					lineEdit->setValidator(validatorPort);
					return lineEdit;
				}
				break;
				case 4:
				{
					QComboBox* combox = new QComboBox(parent);
					combox->addItem("0:无效0");
					combox->addItem("1:合");
					combox->addItem("2:分");
					combox->addItem("3:无效3");
					combox->setCurrentIndex(0);
					return combox;
				}
				break;
				case 5:
				{
					QRegExp rx("^([0-9]{3}[1-9]|[0-9]{2}[1-9][0-9]{1}|[0-9]{1}[1-9][0-9]{2}|[1-9][0-9]{3})-(((0[13578]|1[02])-(0[1-9]|[12][0-9]|3[01]))|((0[469]|11)-(0[1-9]|[12][0-9]|30))|(02-(0[1-9]|[1][0-9]|2[0-8]))) ((1|0)[0-9]|2[0-3]):([0-5][0-9]):([0-5][0-9]) [0-9]{1,6}$");
					QLineEdit * lineEdit = new QLineEdit(parent);
					QValidator *validatorPort=new QRegExpValidator(rx, lineEdit);
					lineEdit->setValidator(validatorPort);
					return lineEdit;
				#if 0
					QLineEdit* lineEdit = new QLineEdit(parent);
					lineEdit->setToolTip("时间格式：yyyy-MM-dd hh:mm:ss zzzuuu");
					return lineEdit;
				#endif
				}
				break;
				case 6:
				{
					QComboBox* combox = new QComboBox(parent);
					combox->addItem("0:False");
					combox->addItem("1:True");
					combox->setCurrentIndex(0);
					return combox;
				}
				break;
				case 7:
				{
					//\b0[xX][0-9a-fA-F]{0,4}\b
					QRegExp rx("0[xX][0-9a-fA-F]{0,4}");
					QLineEdit * lineEdit = new QLineEdit(parent);
					QValidator *validatorPort=new QRegExpValidator(rx, lineEdit);
					lineEdit->setValidator(validatorPort);
					return lineEdit;
				}
				break;
				case 8:
				{
					QLineEdit * lineEdit = new QLineEdit(parent);
					return lineEdit;
				}
				break;
				}
			*/
				QLineEdit * lineEdit = new QLineEdit(parent);
				return lineEdit;
				break;
		}
	case 7:
		{
			QComboBox* combox = new QComboBox(parent);
			combox->addItem("BOOLEAN");
			combox->addItem("INT8");
			combox->addItem("INT16");
			combox->addItem("INT24");
			combox->addItem("INT32");
			combox->addItem("INT128");
			combox->addItem("INT8U");
			combox->addItem("INT16U");
			combox->addItem("INT24U");
			combox->addItem("INT32U");
			combox->addItem("FLOAT32");
			combox->addItem("FLOAT64");
			combox->addItem("Enum");
			combox->addItem("Dbpos");
			combox->addItem("Tcmd");
			combox->addItem("Quality");
			combox->addItem("Timestamp");
			combox->addItem("VisString32");
			combox->addItem("VisString64");
			combox->addItem("VisString255");
			combox->addItem("Octet64");
			combox->addItem("Struct");
			combox->addItem("EntryTime");
			combox->addItem("Unicode255");
			combox->setCurrentIndex(0);
			return combox;
			break;
		}
	case 8:
		{//0:未知  1:字符串  2：整数  3：浮点数  4：bitstring  5:utctime  6:bool  7:品质  8:Struct ...
			QComboBox* combox = new QComboBox(parent);
			combox->addItem("0:未知");
			combox->addItem("1:字符串");
			combox->addItem("2:整数");
			combox->addItem("3:浮点数");
			combox->addItem("4:bitstring");
			combox->addItem("5:utctime");
			combox->addItem("6:bool");
			combox->addItem("7:品质");
			combox->addItem("8:Struct");
			combox->setCurrentIndex(0);
			return combox;
			break;
		}
	case 12:
	case 13:
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

void CDataSetCtrlDelegate::updateEditorGeometry(QWidget *editor,const QStyleOptionViewItem &option,const QModelIndex &) const
{
	if(editor)
	{
		editor->setGeometry(option.rect);
	}
}

void CDataSetCtrlDelegate::setEditorData(QWidget *editor,const QModelIndex &index) const
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

void CDataSetCtrlDelegate::setModelData(QWidget *editor,QAbstractItemModel *model,const QModelIndex &index) const
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


///////////////////////////////////////////////
panel_sim_modify_dataset::panel_sim_modify_dataset(QWidget *parent)
	: CBaseView(parent)
{
	ui.setupUi(this);
	m_pCurTreeItem = NULL;
	m_bHasChanged = false;
	//initTable();
	CGuiApplication *pApp = (CGuiApplication*)SApplication::GetPtr();
	m_pSimConfig = &pApp->m_SimConfig;
	m_pSimMmsServer = &pApp->m_SimManager.m_MmsServer;
	//m_pSimManager = &pApp->m_SimManager;
	//onRefreshData();
	initTable();
	connect(ui.tableWidget,SIGNAL(itemChanged(QTableWidgetItem *)),this,SLOT(slotItemChanged(QTableWidgetItem *)));
}

panel_sim_modify_dataset::~panel_sim_modify_dataset()
{

}

void panel_sim_modify_dataset::OnRefresh()
{
	m_bHasChanged = false;
	if (m_pCurTreeItem == NULL)
	{
		return;
	}
	QString qstr = m_pCurTreeItem->data(0,Qt::UserRole+1).toString();
	m_pCurIedDataSet = (stuSclVtIedDataset*)atol(qstr.toStdString().data());
	if (m_pCurIedDataSet == NULL)
	{
		return;
	}
	ui.tableWidget->blockSignals(true);
	refreshData();
	ui.tableWidget->blockSignals(false);
}

void panel_sim_modify_dataset::initTable()
{
	ui.tableWidget->clear();
	QStringList labels;
	ui.tableWidget->setColumnCount(14);
	labels << "通道号" << "通道类型"<<"逻辑节点类型定义"<<"数据类型" << "逻辑节点类型描述"
		<<"doi描述"<<"dai描述"<<"属性基本类型"<<"基本类型编号"<<"单位"<<"路径"<<"值"<<"一次额定"<<"二次额定"; 
	ui.tableWidget->setHorizontalHeaderLabels(labels);
	ui.tableWidget->setColumnWidth(0,100);
	ui.tableWidget->setColumnWidth(1,200);
	ui.tableWidget->setColumnWidth(2,100);
	ui.tableWidget->setColumnWidth(3,100);
	ui.tableWidget->setColumnWidth(4,100);
	ui.tableWidget->setColumnWidth(5,150);
	ui.tableWidget->setColumnWidth(6,150);
	ui.tableWidget->setColumnWidth(7,80);
	ui.tableWidget->setColumnWidth(8,100);
	ui.tableWidget->setColumnWidth(9,80);
	ui.tableWidget->setColumnWidth(10,150);
	ui.tableWidget->setColumnWidth(11,100);
	ui.tableWidget->setColumnWidth(12,100);
	ui.tableWidget->setColumnWidth(13,100);
	ui.tableWidget->hideColumn(2);
	ui.tableWidget->hideColumn(3);
	ui.tableWidget->hideColumn(4);
	ui.tableWidget->hideColumn(12);
	ui.tableWidget->hideColumn(13);
	ui.tableWidget->setItemDelegate(new CDataSetCtrlDelegate(this,ui.tableWidget));
	SQt::SetTableWidgetStyle(ui.tableWidget);
	ui.tableWidget->resizeColumnsToContents();
}

void panel_sim_modify_dataset::refreshData()
{
	ui.tableWidget->clearContents();
	ui.tableWidget->setRowCount(0);
	for(int i = 0; i < m_pCurIedDataSet->m_Fcda.count(); i ++)
	{
		stuSclVtIedDatasetFcda* pCurFcda = m_pCurIedDataSet->m_Fcda.at(i);
		if (pCurFcda == NULL)
		{
			continue;
		}

		int iRowCount = ui.tableWidget->rowCount();
		ui.tableWidget->insertRow(iRowCount);
	
		QTableWidgetItem* item = new QTableWidgetItem(QString::number(pCurFcda->chn_no));
		item->setTextAlignment(Qt::AlignCenter);
		item->setFlags(item->flags()&~Qt::ItemIsEditable);
		item->setData(Qt::UserRole,QVariant(SString::toFormat("%ld",(unsigned long)pCurFcda).data()));
		ui.tableWidget->setItem(iRowCount,0,item);

		QString strChnType;
		switch(pCurFcda->chn_type)
		{
			case 0:
				strChnType = "0:未知";
				break;
			case 1:
				strChnType = "1:交流电压";
				break;
			case 2:
				strChnType = "2:交流电流";
				break;
			case 3:
				strChnType = "3:直流量";
				break;
			case 4:
				strChnType = "4:额定延时时间";
				break;
			default:
				strChnType = "0:未知";
				break;
		}
		item = new QTableWidgetItem(strChnType);
		item->setTextAlignment(Qt::AlignCenter);
		item->setFlags(item->flags()&~Qt::ItemIsEditable);
		ui.tableWidget->setItem(iRowCount,1,item);

		item = new QTableWidgetItem(pCurFcda->ln_type);
		item->setTextAlignment(Qt::AlignCenter);
		item->setFlags(item->flags()&~Qt::ItemIsEditable);
		ui.tableWidget->setItem(iRowCount,2,item);

		item = new QTableWidgetItem(pCurFcda->do_type);
		item->setTextAlignment(Qt::AlignCenter);
		item->setFlags(item->flags()&~Qt::ItemIsEditable);
		ui.tableWidget->setItem(iRowCount,3,item);

		item = new QTableWidgetItem(pCurFcda->ln_desc);
		item->setTextAlignment(Qt::AlignCenter);
		item->setFlags(item->flags()&~Qt::ItemIsEditable);
		ui.tableWidget->setItem(iRowCount,4,item);

		item = new QTableWidgetItem(pCurFcda->doi_desc);
		item->setTextAlignment(Qt::AlignCenter);
		item->setFlags(item->flags()&~Qt::ItemIsEditable);
		ui.tableWidget->setItem(iRowCount,5,item);

		item = new QTableWidgetItem(pCurFcda->dai_desc);
		item->setTextAlignment(Qt::AlignCenter);
		item->setFlags(item->flags()&~Qt::ItemIsEditable);
		ui.tableWidget->setItem(iRowCount,6,item);


		item = new QTableWidgetItem(pCurFcda->bType);
		item->setTextAlignment(Qt::AlignCenter);
		item->setFlags(item->flags()&~Qt::ItemIsEditable);
		ui.tableWidget->setItem(iRowCount,7,item);

		QString strVType;
		switch(pCurFcda->vtype)
		{
			case 0:
				strVType = "0:未知";
				break;
			case 1:
				strVType = "1:字符串";
				break;
			case 2:
				strVType = "2:整数";
				break;
			case 3:
				strVType = "3:浮点数";
				break;
			case 4:
				strVType = "4:bitstring";
				break;
			case 5:
				strVType = "5:utctime";
				break;
			case 6:
				strVType = "6:bool";
				break;
			case 7:
				strVType = "7:Struct";
				break;
			default:
				strVType = "0:未知";
				break;
		}
		item = new QTableWidgetItem(strVType);
		item->setTextAlignment(Qt::AlignCenter);
		ui.tableWidget->setItem(iRowCount,8,item);

		item = new QTableWidgetItem(pCurFcda->dime);
		item->setTextAlignment(Qt::AlignCenter);
		item->setFlags(item->flags()&~Qt::ItemIsEditable);
		ui.tableWidget->setItem(iRowCount,9,item);

		item = new QTableWidgetItem(pCurFcda->path.data());
		item->setTextAlignment(Qt::AlignCenter);
		item->setFlags(item->flags()&~Qt::ItemIsEditable);
		ui.tableWidget->setItem(iRowCount,10,item);

		QString strValue;
		/*
		switch(pCurFcda->vtype)
		{
		case 0:
		case 1:
		case 2:
		case 3:
			strValue = pCurFcda->value.data();
			break;
		case 4:
			{
				switch(pCurFcda->value.toInt())
				{
				case 0:
					strValue = "0:无效0";
					break;
				case 1:
					strValue = "1:合";
					break;
				case 2:
					strValue = "2:分";
					break;
				case 3:
					strValue = "3:无效3";
					break;
				}
			}
			break;
		case 5:
			{
				if (pCurFcda->value.isEmpty())
				{
					strValue = "";
				} else 
				{
					SString t = SString::GetAttributeValue(pCurFcda->value,"t");

					if(pCurFcda->value.length() == 19)
						strValue = t.data();
					else 
					{
						SString strUsec = SString::GetAttributeValue(pCurFcda->value,"usec");
						strValue = tr("%1 %2").arg(t.data()).arg(strUsec.data());
					}
				}
			}
			break;
		case 6:
			{
				if (pCurFcda->value.isEmpty())
				{
					strValue = "";
				} else
				{
					if (pCurFcda->value.toInt() == 0)
					{
						strValue = "0:False";
					} else 
					{
						strValue = "1:True";
					}
				}
				
			}
			break;
		case 7:
		case 8:
			{
				strValue = pCurFcda->value.data();
			}
			break;
		}
		*/
		if(m_pSimMmsServer->IsRuning())
			strValue = m_pSimMmsServer->GetLeafVarValueByPath(m_sMmsDsIedName+pCurFcda->path);
		item = new QTableWidgetItem(strValue);
		item->setTextAlignment(Qt::AlignCenter);
		ui.tableWidget->setItem(iRowCount,11,item);

		item = new QTableWidgetItem(QString::number(pCurFcda->primary_rated,'f',3));
		item->setTextAlignment(Qt::AlignCenter);
		item->setFlags(item->flags()&~Qt::ItemIsEditable);
		ui.tableWidget->setItem(iRowCount,12,item);

		item = new QTableWidgetItem(QString::number(pCurFcda->secondary_rated,'f',3));
		item->setTextAlignment(Qt::AlignCenter);
		item->setFlags(item->flags()&~Qt::ItemIsEditable);
		ui.tableWidget->setItem(iRowCount,13,item);
	}
	ui.tableWidget->resizeColumnsToContents();
}

void panel_sim_modify_dataset::slotItemChanged(QTableWidgetItem *item)
{
	int row = item->row();
	int col = item->column();
	QString strAddr = ui.tableWidget->item(row,0)->data(Qt::UserRole).toString();
	stuSclVtIedDatasetFcda* pCurFcda = (stuSclVtIedDatasetFcda*)strAddr.toLong();

	if (col == 11)
	{
		QString strCurText = item->text();	
		/*
		switch(pCurFcda->vtype)
		{
		case 0:
		case 1:
		case 2:
		case 3:
			pCurFcda->value = strCurText.toStdString().data();
			break;
		case 4:
			{
				pCurFcda->value = strCurText.split(":").at(0).toStdString().data();
			}
			break;
		case 5:
			{
				if (strCurText.isEmpty())
				{
					pCurFcda->value = "";
				}
				else
				{
					strCurText = strCurText.trimmed();
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
						}
					} else 
					{
						strTime = strCurText;
						strUsec = "";
					}
					strValue = tr("t=%1;usec=%2;").arg(strTime).arg(strUsec);
					pCurFcda->value = strValue.toStdString().data();
				}
			}
			break;
		case 6:
			{
				if (strCurText.isEmpty())
				{
					pCurFcda->value = "";
				} else
				{
					pCurFcda->value = strCurText.split(":").at(0).toStdString().data();
				}
				
			}
			break;
		case 7:
		case 8:
			{
				pCurFcda->value = strCurText.toStdString().data();
			}
			break;
		}
		*/
		if(!m_pSimMmsServer->IsRuning())
		{
			SQt::ShowErrorBox("提示","MMS服务尚未启动!");
			return;
		}

		QString mmspath = ui.tableWidget->item(row,10)->text();
		SString path=m_sMmsDsIedName+mmspath.toStdString().data();
		SString sVal = strCurText.toStdString().data();
		pCurFcda->value = sVal;
		if(!m_pSimMmsServer->SetLeafVarValueByPath(path,sVal))
		{
			SQt::ShowErrorBox("提示","写入MMS服务节点时失败!");
			return;
		}

// 		SPtrList<stuLeafMap> leafs;
// 		SString sVal = m_pSimMmsServer->GetLeafVarValueByPath(path);
// 		SQt::ShowInfoBox("提示",SString::toFormat("未找到指定节点:%s",sVal.data()).data());
		//m_pSimConfig->Save();
	} else if (col == 8)
	{
		QString strCurText = item->text();
		QString strIndex = strCurText.split(":").at(0);
		pCurFcda->vtype =  strIndex.toInt();

		QString mmspath = ui.tableWidget->item(row,7)->text();
		SString path=m_sMmsDsIedName+mmspath.toStdString().data();
		
		stuLeafMap *pMap = m_pSimMmsServer->SearchLeafMap(path.data());
		if(pMap == NULL)
		{
			SQt::ShowInfoBox("提示",SString::toFormat("未找到指定节点:%s",path.data()).data());
			return;
		}
		//m_pSimConfig->Save();

	}
}

