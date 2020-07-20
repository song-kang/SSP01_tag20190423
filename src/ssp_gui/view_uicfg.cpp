#include "view_uicfg.h"
#include "ssp_gui.h"
#include "gui/qt/SSvgEditorWnd.h"
#define  UICFG_WND_COL_CLS_NAME        0
#define  UICFG_WND_COL_WND_SN          1
#define  UICFG_WND_COL_WND_NAME        2
#define  UICFG_WND_COL_CREATE_AUTHOR   3
#define  UICFG_WND_COL_CREATE_TIME     4
#define  UICFG_WND_COL_MODIFY_AUTHOR   5
#define  UICFG_WND_COL_MODIFY_TIME     6
#define  UICFG_WND_COL_WND_TYPE        7
#define  UICFG_WND_COL_SVG_FILE        8
#define  UICFG_WND_COL_DATASET         9
#define  UICFG_WND_COL_DS_COND         10
#define  UICFG_WND_COL_REFRESH_SEC     11

view_uicfg::view_uicfg(QWidget *parent)
	: CBaseView(parent)
{
	ui.setupUi(this);
	m_tableName="t_ssp_uicfg_wnd";
	SQt::SetTableWidgetStyle(ui.tableWidget);
	m_sspgui=ssp_gui::GetPtr();
	m_dbOper=DB;
	m_svgEditWnd=NULL;

	initTable();
	initCombo();
	initDataInfoMap();
	refreshTable("全部");
	connect(ui.pushButton_copy,SIGNAL(clicked()),this,SLOT(onPushButton_copy()));
	connect(ui.comboBox_cls,SIGNAL(currentIndexChanged(QString)),this,SLOT(refreshTable(QString)));
}

view_uicfg::~view_uicfg()
{

}
void view_uicfg::initCombo()
{
	ui.comboBox_cls->blockSignals(true);
	SRecordset cls;
	m_dbOper->RetrieveRecordset("select distinct cls_name from t_ssp_uicfg_wnd order by cls_name",cls);
	ui.comboBox_cls->clear();
	ui.comboBox_cls->addItem("全部");
	for(int i=0;i<cls.GetRows();i++)
	{
		QString tmp=cls.GetValue(i,0).data();
		if(tmp!="")
		{
			ui.comboBox_cls->addItem(tmp);
		}
	}
	ui.comboBox_cls->blockSignals(false);
}

bool view_uicfg::InitWidget(SString sWname)
{
	S_UNUSED(sWname);
	SSP_GUI->ShowWaiting();
	ui.tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
	ui.pushButton_2->setEnabled(false);
	ui.pushButton->setEnabled(false);
	//setDataToWidget();
	SSP_GUI->HideWaiting();
	return true;
}
//屏蔽鼠标滚轮
bool view_uicfg::eventFilter(QObject *obj, QEvent *event)
{
	//QString objname=obj->objectName();
	if(event->type() == QEvent::Wheel)
	{
		return true;
	}
	else
	{
		return QWidget::eventFilter(obj, event);
	}
}

QString view_uicfg::getItemValue(int row, int col)
{
	QString str = ui.tableWidget->item(row,col)->text();
	return str;
}
bool view_uicfg::getCheckBoxStateAndComBoxText(QString& strComBoxText)
{
	strComBoxText = ui.comboBox_cls->currentText();
	return ui.checkBox_useCls->isChecked();
}

void view_uicfg::addTableWidgetOperItem(int row)
{
	QWidget* pwidget = ui.tableWidget->cellWidget(row,UICFG_WND_COL_SVG_FILE);
	if (pwidget)
	{
		return;
	}
	QWidget * widget=new QWidget(this);
	QGridLayout *gridLayout;
	QPushButton *uploadRowBtn;
	QPushButton *downloadRowBtn;
	QPushButton *editorRowBtn;

	gridLayout = new QGridLayout(widget);
	gridLayout->setSpacing(0);
	gridLayout->setContentsMargins(0, 0, 0, 0);
	uploadRowBtn = new QPushButton(widget);
	uploadRowBtn->setObjectName(QString::fromUtf8("pushButton"));
	QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
	sizePolicy.setHorizontalStretch(0);
	sizePolicy.setVerticalStretch(0);
	sizePolicy.setHeightForWidth(uploadRowBtn->sizePolicy().hasHeightForWidth());
	uploadRowBtn->setSizePolicy(sizePolicy);
	gridLayout->addWidget(uploadRowBtn, 0, 0, 1, 1);

	downloadRowBtn = new mPushButton(widget);
	downloadRowBtn->setObjectName(QString::fromUtf8("pushButton_2"));
	sizePolicy.setHeightForWidth(downloadRowBtn->sizePolicy().hasHeightForWidth());
	downloadRowBtn->setSizePolicy(sizePolicy);
	gridLayout->addWidget(downloadRowBtn, 0, 1, 1, 1);

	editorRowBtn = new mPushButton(widget);
	editorRowBtn->setObjectName(QString::fromUtf8("pushButton_3"));
	sizePolicy.setHeightForWidth(editorRowBtn->sizePolicy().hasHeightForWidth());
	editorRowBtn->setSizePolicy(sizePolicy);
	gridLayout->addWidget(editorRowBtn, 0, 2, 1, 1);

	uploadRowBtn->setText(tr("上传..."));
	downloadRowBtn->setText(tr("下载..."));
	editorRowBtn->setText(tr("编辑..."));
	widget->setLayout(gridLayout); 
	ui.tableWidget->setCellWidget(row,UICFG_WND_COL_SVG_FILE,widget);
	connect(uploadRowBtn,SIGNAL(clicked()),this,SLOT(slotupLoadRowClicked()));
	connect(downloadRowBtn,SIGNAL(clicked()),this,SLOT(slotDownLoadRowClicked()));
	connect(editorRowBtn,SIGNAL(clicked()),this,SLOT(slotEditorRowClicked()));
}

int	view_uicfg::getRowByPushButton(QObject *object)
{
	QPushButton * sender = qobject_cast<QPushButton *>(object);
	if (!sender)
		return -1;

	int row = -1;
	QWidget *widget = sender->parentWidget();
	for (int i = 0; i < ui.tableWidget->rowCount(); i++)
	{
		QWidget * w = ui.tableWidget->cellWidget(i,UICFG_WND_COL_SVG_FILE);
		if (w!= NULL && w == widget)
		{
			row = i;
			break;
		}
	}

	return row;
}

void view_uicfg::slotupLoadRowClicked()
{
	QObject * object = QObject::sender();
	if (!object)
		return;

	int row = getRowByPushButton(object);
	if (row < 0)
	{
		return;
	}
	QStringList filters;
	filters << "Sql files (*.svg)";
	QFileDialog dialog;
	dialog.setNameFilters(filters);
	QString fName;
	dialog.setAcceptMode(QFileDialog::AcceptOpen);
	if (dialog.exec())
	{

		fName = dialog.selectedFiles().first();
		//delete dialog;
		SString sWhere = SString::toFormat("wnd_sn=%d",ui.tableWidget->item(row,UICFG_WND_COL_WND_SN)->text().toInt());
		if(!m_dbOper->UpdateLobFromFile(m_tableName,"svg_file",sWhere,fName.toStdString().data()))
		{
			QMessageBox::warning(this,tr("警告"),tr("从[%1]加载文件失败").arg(fName),QMessageBox::Ok);
			return;		
		}
	}
	
}
void view_uicfg::slotDownLoadRowClicked()
{
	QObject * object = QObject::sender();
	if (!object)
		return;

	int row = getRowByPushButton(object);
	if (row < 0)
	{
		return;
	}
	QStringList filters;
	filters << "Sql files (*.svg)";
	QFileDialog dialog;
	dialog.setNameFilters(filters);
	QString fName;
	dialog.setAcceptMode(QFileDialog::AcceptSave);
	if (dialog.exec())
	{
		fName = dialog.selectedFiles().first();
		if (fName.indexOf(".svg") < 0)
		{
			fName += ".svg";
		}
		SString sWhere = SString::toFormat("wnd_sn=%d",ui.tableWidget->item(row, UICFG_WND_COL_WND_SN)->text().toInt());
		if(!m_dbOper->ReadLobToFile(m_tableName,"svg_file",sWhere,fName.toStdString().data()))
		{
			QMessageBox::warning(this,tr("警告"),tr("加载数据到文件[%1]失败").arg(fName),QMessageBox::Ok);
			return;		
		}
	}
}
void view_uicfg::slotEditorRowClicked()
{
	QObject * object = QObject::sender();
	if (!object)
		return;

	int row = getRowByPushButton(object);
	if (row < 0)
	{
		return;
	}
	unsigned char*buff = NULL;
	int len = 0;
	SString sSave="";
	SString sWhere = SString::toFormat("wnd_sn=%d",ui.tableWidget->item(row, UICFG_WND_COL_WND_SN)->text().toInt());
	m_dbOper->ReadLobToMem(m_tableName,"svg_file",sWhere,buff,len);
	if (m_svgEditWnd==NULL)
	{
		m_svgEditWnd=new DynamicSSvgEditorWnd();
	}
	m_svgEditWnd->setData(m_dbOper,ui.tableWidget->item(row,UICFG_WND_COL_DATASET)->text().toStdString().data(),(char *)buff);
	m_svgEditWnd->setWindowState(Qt::WindowMaximized);
	m_svgEditWnd->exec();
	m_svgEditWnd->getSaveBuff(sSave);
	if(sSave != "")
		m_dbOper->UpdateLobFromMem(m_tableName,"svg_file",sWhere,(unsigned char*)sSave.data(),sSave.size());
	delete m_svgEditWnd;
	m_svgEditWnd=NULL;
	m_dbOper->FreeLobMem(buff);
}

void view_uicfg::on_pushButton_pressed()
{
	SString sUser = m_sspgui->GetOperUser();
	int newAddIndex=getMaxIndex() + 1;
	int rowNum = ui.tableWidget->rowCount();
	SString clsNam="";
	if(ui.comboBox_cls->currentText()!="全部")
		clsNam=ui.comboBox_cls->currentText().toStdString().data();
	SDateTime curTime = SDateTime::currentDateTime();
	SString stime = curTime.toString("yyyy-MM-dd hh:mm:ss");

	SString sSql = SString::toFormat("insert into t_ssp_uicfg_wnd (wnd_sn,wnd_name,create_author,create_time,modify_author,modify_time,wnd_type,svg_file,dataset,ds_cond,refresh_sec,cls_name) \
									 values(%d,'','%s',%d,'',NULL,1,NULL,'','',0,'%s')",
		newAddIndex, sUser.data(),curTime.soc(),clsNam.data());

	if (!m_dbOper->ExecuteSQL(sSql))
	{
		QMessageBox::information(NULL,"tip",tr("数据库插入失败[%1]!").arg(sSql.data()));
		return;		
	}

	ui.tableWidget->blockSignals(true);
	stUicfgWnd stUwnd;
	stUwnd.cls_name = clsNam.data();
	stUwnd.wnd_sn = QString::number(newAddIndex);
	stUwnd.wnd_name = "";
	stUwnd.create_author = sUser.data();
	stUwnd.create_time = stime.data();
	stUwnd.modify_author = "";
	stUwnd.modify_time = "";
	stUwnd.wnd_type = "SVG图形窗口";
	stUwnd.svg_file = "<BLOB>";
	stUwnd.dataset = "";
	stUwnd.ds_cond = "";
	stUwnd.refresh_sec = "";
	insertNewRow(&stUwnd, rowNum);
	ui.tableWidget->blockSignals(false);
}
void view_uicfg::onPushButton_copy()
{
	//先判断时候有选择区域
	QList<QTableWidgetSelectionRange> lSelectedRange = ui.tableWidget->selectedRanges();
	int selCount = lSelectedRange.count();//获取选择区域的个数
	if (selCount < 1)
	{
		QMessageBox::information(NULL,"提示",tr("当前无选择行对象，请先选择行!"));
		return;
	}

	bool ok;  
	int cpyNum= QInputDialog::getInteger(this,tr("复制个数"),  
		tr("输入复制个数:"), 1,1,150,1,&ok); 
	if(!ok)
		return;

	ui.tableWidget->blockSignals(true);
	int curAddMaxindex= getMaxIndex() + 1;
	int curAddRowSn = ui.tableWidget->rowCount();
	SString sUser = m_sspgui->GetOperUser();
	for(int k = 0; k < cpyNum; k++)
	{
		for(int i=0;i < selCount;i++)//选择区域个数
		{
			int topRow=lSelectedRange.at(i).topRow();//获取当前选择区域的起始行号
			int bottomRow=lSelectedRange.at(i).bottomRow();//获取当前选择区域的终止行号
			for(int j=topRow; j <= bottomRow;j++)
			{
				SDateTime curTime = SDateTime::currentDateTime();
				SString stime = curTime.toString("yyyy-MM-dd hh:mm:ss");
				int curSelRow = j;
				QString strCurSelRowWndSn = ui.tableWidget->item(curSelRow,UICFG_WND_COL_WND_SN)->text();
				//先做数据插入，插入成功后才在页面新增
				SString sSql = SString::toFormat("insert into t_ssp_uicfg_wnd (wnd_sn,wnd_name,create_author,create_time,modify_author,modify_time,wnd_type,svg_file,dataset,ds_cond,refresh_sec,cls_name) \
												 select %d,wnd_name,'%s',%d,'',NULL,wnd_type,svg_file,dataset,ds_cond,refresh_sec,cls_name from t_ssp_uicfg_wnd where wnd_sn=%s",
												 curAddMaxindex, sUser.data(),curTime.soc(),strCurSelRowWndSn.toStdString().c_str());
				if (!m_dbOper->ExecuteSQL(sSql))
				{
					QMessageBox::information(NULL,"tip",tr("添加记录失败[%1]!").arg(sSql.data()));
					ui.tableWidget->blockSignals(false);
					return;
				}
				stUicfgWnd stUwnd;
				stUwnd.cls_name = ui.tableWidget->item(curSelRow,UICFG_WND_COL_CLS_NAME)->text();
				stUwnd.wnd_sn = QString::number(curAddMaxindex);
				stUwnd.wnd_name = ui.tableWidget->item(curSelRow,UICFG_WND_COL_WND_NAME)->text();
				stUwnd.create_author = sUser.data();
				stUwnd.create_time = stime.data();
				stUwnd.modify_author = "";
				stUwnd.modify_time = "";
				stUwnd.wnd_type = ui.tableWidget->item(curSelRow,UICFG_WND_COL_WND_TYPE)->text();
				if (ui.tableWidget->item(curSelRow,UICFG_WND_COL_WND_TYPE)->text() == "SVG图形窗口")
				{
					stUwnd.svg_file = "<BLOB>";
				} else {
					stUwnd.svg_file = "<NULL>";
				}
				stUwnd.dataset = ui.tableWidget->item(curSelRow,UICFG_WND_COL_DATASET)->text();
				stUwnd.ds_cond = ui.tableWidget->item(curSelRow,UICFG_WND_COL_DS_COND)->text();
				stUwnd.refresh_sec = ui.tableWidget->item(curSelRow,UICFG_WND_COL_REFRESH_SEC)->text();
				insertNewRow(&stUwnd, curAddRowSn);

				curAddRowSn ++;
				curAddMaxindex ++;
			}
		}
	}
	ui.tableWidget->blockSignals(false);
	ui.tableWidget->resizeColumnsToContents();
	ui.tableWidget->setColumnWidth(UICFG_WND_COL_SVG_FILE,48*3);
}
void view_uicfg::on_pushButton_2_pressed()
{
	QList<QTableWidgetSelectionRange> lSelectedRange = ui.tableWidget->selectedRanges();
	SString sSql;
	int count = lSelectedRange.count();//获取选择区域的个数
	if (count < 1)
	{
		QMessageBox::warning(this,tr("警告"),tr("没有选择区域，请先选择删除对象"),QMessageBox::Ok);
		return;
	}

	int i_ret = QMessageBox::warning(this,tr("警告"),tr("是否删除所选行？"),QMessageBox::Yes,QMessageBox::No);
	if (i_ret==QMessageBox::No)
	{
		return;
	}
	ui.tableWidget->blockSignals(true);
	for(int i=0;i < count;i++)
	{
		int topRow=lSelectedRange.at(i).topRow();//获取当前选择区域的起始行号
		int bottomRow=lSelectedRange.at(i).bottomRow();//获取当前选择区域的终止行号
		for(int j=topRow; j <= bottomRow;j++)
		{
			sSql = SString::toFormat("delete from t_ssp_uicfg_wnd where wnd_sn=%s",
				ui.tableWidget->item(j,UICFG_WND_COL_WND_SN)->text().toStdString().c_str());
			if (!m_dbOper->Execute(sSql))
			{
				QMessageBox::information(NULL,"tip",tr("删除数据失败![%1]").arg(sSql.data()));
				ui.tableWidget->blockSignals(false);
				return;
			}
			ui.tableWidget->removeRow(j);
		}
	}
	ui.tableWidget->blockSignals(false);
	initCombo();
	return;
}
void view_uicfg::on_tableWidget_itemDoubleClicked(QTableWidgetItem * item)
{
	int icol = item->column();
	if (icol == UICFG_WND_COL_WND_SN || icol == UICFG_WND_COL_CREATE_AUTHOR 
		|| icol == UICFG_WND_COL_CREATE_TIME || icol == UICFG_WND_COL_MODIFY_AUTHOR
		|| icol == UICFG_WND_COL_MODIFY_TIME || icol == UICFG_WND_COL_SVG_FILE)
	{
		return;
	}
	//if (m_bCellchange[item->column()])
	{
		ui.tableWidget->editItem(item);
	}
}
void view_uicfg::on_tableWidget_itemChanged(QTableWidgetItem * item)
{
	int icol = item->column();
	int iRow = item->row();
	if (iRow < 0 || icol < 0)
	{
		return;
	}
	SString sUser = m_sspgui->GetOperUser();
	SDateTime curTime = SDateTime::currentDateTime();

	QString strWndSn = ui.tableWidget->item(iRow,UICFG_WND_COL_WND_SN)->text();
	ui.tableWidget->blockSignals(true);
	SString sql;
	if (icol == UICFG_WND_COL_WND_TYPE)
	{
		QString str = item->text();
		int iType = wndTypeMap.key(str).toInt();
		if(str == "SVG图形窗口")
		{
			addTableWidgetOperItem(iRow);
		} else
		{
			QWidget* pwidget = ui.tableWidget->cellWidget(iRow,UICFG_WND_COL_SVG_FILE);
			if (pwidget)
			{
				ui.tableWidget->removeCellWidget(iRow,UICFG_WND_COL_SVG_FILE);
			}
			QTableWidgetItem* item = new QTableWidgetItem();
			item->setText("<NULL>");
			item->setFlags(item->flags() & (~Qt::ItemIsEditable));	//设置item项不可编辑
			ui.tableWidget->setItem(iRow,UICFG_WND_COL_SVG_FILE,item);
			//ui.tableWidget->item(iRow,UICFG_WND_COL_DATASET)->setText("");
		}
		ui.tableWidget->item(iRow,UICFG_WND_COL_DATASET)->setText("");
		//当窗口类型改变时，要把dataset清空
		sql =  SString::toFormat("update t_ssp_uicfg_wnd set wnd_type=%d,dataset=' ',modify_author='%s',modify_time=%d where wnd_sn=%s",
			iType,sUser.data(),curTime.soc(),strWndSn.toStdString().c_str());
	} else if (icol == UICFG_WND_COL_WND_NAME)
	{
		sql = SString::toFormat("update t_ssp_uicfg_wnd set wnd_name='%s',modify_author='%s',modify_time='%d' where wnd_sn=%s",
			item->text().toStdString().c_str(),sUser.data(),curTime.soc(),strWndSn.toStdString().c_str());
	} else if (icol == UICFG_WND_COL_CLS_NAME)
	{
		sql = SString::toFormat("update t_ssp_uicfg_wnd set cls_name='%s',modify_author='%s',modify_time='%d' where wnd_sn=%s",
			item->text().toStdString().c_str(),sUser.data(),curTime.soc(),strWndSn.toStdString().c_str());
	}else if (icol == UICFG_WND_COL_DATASET)
	{
		QString strDatasetName; 
		if (!item->text().isEmpty())
		{
			QString strWndType = ui.tableWidget->item(iRow,UICFG_WND_COL_WND_TYPE)->text();
			if (strWndType == "报表窗口")
			{
				strDatasetName = reportMap.key(item->text());
			} else {
				strDatasetName = dataSetMap.key(item->text());
			}
		} else {
			strDatasetName = item->text();
		}
		
		sql = SString::toFormat("update t_ssp_uicfg_wnd set dataset='%s',modify_author='%s',modify_time='%d' where wnd_sn=%s",
			strDatasetName.toStdString().c_str(),sUser.data(),curTime.soc(),strWndSn.toStdString().c_str());
	}else if (icol == UICFG_WND_COL_DS_COND)
	{
		sql = SString::toFormat("update t_ssp_uicfg_wnd set ds_cond=%s,modify_author='%s',modify_time='%d' where wnd_sn=%s",
			item->text().toStdString().c_str(),sUser.data(),curTime.soc(),strWndSn.toStdString().c_str());
	}else if (icol == UICFG_WND_COL_REFRESH_SEC)
	{
		sql = SString::toFormat("update t_ssp_uicfg_wnd set refresh_sec='%s',modify_author='%s',modify_time='%d' where wnd_sn=%s",
			item->text().toStdString().c_str(),sUser.data(),curTime.soc(),strWndSn.toStdString().c_str());
	}

	if (!m_dbOper->ExecuteSQL(sql))
	{
		QMessageBox::information(NULL,"tip",tr("数据库更新失败[%1]!").arg(sql.data()));
		ui.tableWidget->blockSignals(false);
		return;
	} else {
		//更新table表中修改作者和时间列
		ui.tableWidget->item(iRow,UICFG_WND_COL_MODIFY_AUTHOR)->setText(sUser.data());
		ui.tableWidget->item(iRow,UICFG_WND_COL_MODIFY_TIME)->setText(curTime.toString("yyyy-MM-dd hh:mm:ss").data());
	}
	ui.tableWidget->blockSignals(false);
	ui.tableWidget->resizeColumnsToContents();
	ui.tableWidget->setColumnWidth(UICFG_WND_COL_SVG_FILE,48*3);
	if (icol == UICFG_WND_COL_CLS_NAME)
	{
		initCombo();
	}
	
}
void view_uicfg::on_tableWidget_itemSelectionChanged()
{
	QList<QTableWidgetSelectionRange> lSelectedRange;
	lSelectedRange = ui.tableWidget->selectedRanges();
	int m_colum = ui.tableWidget->columnCount();
	ui.pushButton_2->setEnabled(false);
	foreach (const QTableWidgetSelectionRange &qRange, lSelectedRange)
	{
		if (qRange.leftColumn()==0&&qRange.rightColumn()==m_colum-1)//选中了整行
		{
			ui.pushButton_2->setEnabled(true);
		}else
		{
			ui.pushButton_2->setEnabled(false);
		}
	}
}
void view_uicfg::on_tableWidget_currentItemChanged( QTableWidgetItem * current, QTableWidgetItem * previous )
{	
	if (current && previous)
	{
		if (current->row() == previous->row())
		{
			return;
		}
	}
	ui.tableWidget->blockSignals(true);
	if (previous != NULL)
	{	
		int iPreRow = previous->row();
		if (iPreRow >= 0)
		{
			QString strWndType = ui.tableWidget->item(iPreRow, UICFG_WND_COL_WND_TYPE)->text();
			if (strWndType == tr("SVG图形窗口"))
			{
				QWidget* pwidget = ui.tableWidget->cellWidget(iPreRow,UICFG_WND_COL_SVG_FILE);
				if (pwidget)
				{
					ui.tableWidget->removeCellWidget(iPreRow,UICFG_WND_COL_SVG_FILE);
				}
				QTableWidgetItem* item = new QTableWidgetItem();
				item->setText("<BLOB>");
				item->setFlags(item->flags() & (~Qt::ItemIsEditable));	//设置item项不可编辑
				ui.tableWidget->setItem(iPreRow,UICFG_WND_COL_SVG_FILE,item);
			}
		}
	}
	if(current==NULL)
	{
		ui.tableWidget->blockSignals(false);
		return;
	}
	int iCurRow = current->row();
	if (iCurRow < 0)
	{
		ui.tableWidget->blockSignals(false);
		return;
	}
	QString strWndType = ui.tableWidget->item(iCurRow, UICFG_WND_COL_WND_TYPE)->text();
	if (strWndType == tr("SVG图形窗口") )
	{
		addTableWidgetOperItem(iCurRow);
	}
	ui.tableWidget->blockSignals(false);
}
int view_uicfg::getMaxIndex()
{
	int iRet= m_dbOper->SelectIntoI("select max(wnd_sn) from t_ssp_uicfg_wnd");
	return iRet;
}
void view_uicfg::OnPreShow()
{
	//InitWidget("");
	refreshDataAndReportSetMap();
}

void view_uicfg::initTable()
{
	ui.tableWidget->clear();
	QStringList lHeader;
	lHeader<<tr("分类")<<tr("窗口编号")<<tr("窗口名称")<<tr("初始制作人")<<tr("初始创建时间")<<tr("最后修改人")<<tr("最后修改时间")<<tr("窗口类型")<<tr("svg文件")<<tr("引用")<<tr("数据集表达式")<<tr("窗口刷新周期");
	ui.tableWidget->setColumnCount(lHeader.size());
	ui.tableWidget->setHorizontalHeaderLabels(lHeader);
	//ui.tableWidget->setItemDelegate(new dataWindowDelegate(this));
	ui.tableWidget->setItemDelegateForColumn(UICFG_WND_COL_WND_TYPE,new UicfgWndDelegate(this,UICFG_WND_COL_WND_TYPE));
	ui.tableWidget->setItemDelegateForColumn(UICFG_WND_COL_DATASET,new UicfgWndDelegate(this,UICFG_WND_COL_DATASET));
	//ui.tableWidget->setItemDelegateForColumn(UICFG_WND_COL_WND_TYPE,new UicfgWndDelegate(this,UICFG_WND_COL_WND_TYPE));
	ui.tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);		//整行选择模式
	//tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);		//不可编辑
	//ui.tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);		//单选模式
	ui.tableWidget->setFocusPolicy(Qt::NoFocus);								//去除焦点，无虚框
	//tableWidget->horizontalHeader()->setStretchLastSection(true);			//设置充满表宽度
	//ui.tableWidget->setStyleSheet("selection-background-color:rgb(195,195,0);");	//设置选中背景色
	ui.tableWidget->verticalHeader()->setDefaultSectionSize(24);				//设置行高
	ui.tableWidget->horizontalHeader()->setHighlightSections(false);			//点击表时不对表头行光亮
	ui.tableWidget->setAlternatingRowColors(true);	
}

void view_uicfg::refreshTable(QString strClsName)
{
	SRecordset rs;
	SString sql;
	if (strClsName == "全部")
	{
		sql = "select cls_name,wnd_sn,wnd_name,create_author,create_time,modify_author,modify_time,wnd_type,svg_file,dataset,ds_cond,refresh_sec from t_ssp_uicfg_wnd order by cls_name desc";
	} else {
		sql = SString::toFormat("select cls_name,wnd_sn,wnd_name,create_author,create_time,modify_author,modify_time,wnd_type,svg_file,dataset,ds_cond,refresh_sec from t_ssp_uicfg_wnd where cls_name='%s' or cls_name='' order by cls_name desc",
			strClsName.toStdString().data());	
	}
	
	int ret = m_dbOper->Retrieve(sql,rs);
	if(ret < 0)
	{
		QMessageBox::warning(this,tr("警告"),tr("数据库查询失败[%1]").arg(sql.data()),QMessageBox::Ok);
		return;
	}else if (ret == 0)
	{
		return;
	} else
	{
		ui.tableWidget->blockSignals(true);
		ui.tableWidget->clearContents();
		ui.tableWidget->setRowCount(0);
		int iRow = rs.GetRows();
		//int iColumn = rs.GetColumns();
		for (int i=0; i < iRow; i++)
		{
			if(i%10==0)
				SQt::ProcessUiEvents();
			stUicfgWnd stUwnd;
			stUwnd.cls_name = rs.GetValue(i,0).data();
			stUwnd.wnd_sn = rs.GetValue(i,1).data();
			stUwnd.wnd_name = rs.GetValue(i,2).data();
			stUwnd.create_author = rs.GetValue(i,3).data();

			int iSoc = rs.GetValue(i,4).toInt();
			if (iSoc!=0)
			{
				SDateTime rTime(iSoc);
				stUwnd.create_time = rTime.toString("yyyy-MM-dd hh:mm:ss").data();
			} else {
				stUwnd.create_time = "";
			}

			stUwnd.modify_author = rs.GetValue(i,5).data();
			iSoc = rs.GetValue(i,6).toInt();
			if (iSoc!=0)
			{
				SDateTime rTime(iSoc);
				stUwnd.modify_time = rTime.toString("yyyy-MM-dd hh:mm:ss").data();
			} else {
				stUwnd.modify_time = "";
			}
			QString strtmp = rs.GetValue(i,7).data();
			stUwnd.wnd_type = wndTypeMap[rs.GetValue(i,7).data()];
			if (stUwnd.wnd_type != "SVG图形窗口")
			{
				stUwnd.svg_file = "<NULL>";	
			}else
			{
				stUwnd.svg_file = "<BLOB>";
			}
			if (stUwnd.wnd_type == "报表窗口")
			{
				stUwnd.dataset = reportMap[rs.GetValue(i,9).data()];
			} else {
				stUwnd.dataset = dataSetMap[rs.GetValue(i,9).data()];
			}
			
			//stUwnd.dataset = rs.GetValue(i,9).data();
			stUwnd.ds_cond = rs.GetValue(i,10).data();
			stUwnd.refresh_sec = rs.GetValue(i,11).data();
			insertNewRow(&stUwnd, i);
		}
		ui.tableWidget->blockSignals(false);	
	}

	//ui.tableWidget->setCurrentItem(ui.tableWidget->item(0,0));
	ui.tableWidget->selectRow(0);
	ui.tableWidget->resizeColumnsToContents();
	ui.tableWidget->setColumnWidth(UICFG_WND_COL_SVG_FILE,48*3);
	
}
//刷新数据集的名称和描述映射关系
void view_uicfg::refreshDataAndReportSetMap()
{
	SString sql;
	SRecordset rs;
	reportMap.clear();
	sql = "select report_no,report_name from t_ssp_report_info order by report_no";
	int ret = m_dbOper->Retrieve(sql,rs);
	if(ret < 0)
	{
		QMessageBox::warning(this,tr("警告"),tr("数据库查询失败[%1]").arg(sql.data()),QMessageBox::Ok);
		return;
	}else if (ret == 0)
	{
		
	} else
	{
		for (int i = 0; i < rs.GetRows(); i++)
		{
			reportMap[rs.GetValue(i,0).data()] = rs.GetValue(i,1).data();
		}
	}
	rs.clear();
	sql = "select name,dw_desc from t_ssp_data_window where dstype=2 and dbtype<>1 order by cls_name desc,dw_desc";
	ret = m_dbOper->Retrieve(sql,rs);
	if(ret < 0)
	{
		QMessageBox::warning(this,tr("警告"),tr("数据库查询失败[%1]").arg(sql.data()),QMessageBox::Ok);
		return;
	}else if (ret == 0)
	{

	} else
	{
		for (int i = 0; i < rs.GetRows(); i++)
		{
			dataSetMap[rs.GetValue(i,0).data()] = rs.GetValue(i,1).data();
		}
	}
}
//初始化所有基本数据的映射关系
void view_uicfg::initDataInfoMap()
{
	wndTypeMap.clear();
	wndTypeMap["1"] = tr("SVG图形窗口");
	wndTypeMap["2"] = tr("内存库查询窗口");
	wndTypeMap["3"] = tr("历史库编辑窗口");
	wndTypeMap["4"] = tr("历史库查询窗口");
	wndTypeMap["5"] = tr("报表窗口");
	
	refreshDataAndReportSetMap();
}
void view_uicfg::insertNewRow(stUicfgWnd* stUW, int curRow)
{
	ui.tableWidget->insertRow(curRow);
	QTableWidgetItem * item = new QTableWidgetItem(stUW->cls_name);
	item->setTextAlignment(Qt::AlignCenter);
	ui.tableWidget->setItem(curRow,UICFG_WND_COL_CLS_NAME,item);

	item = new QTableWidgetItem(stUW->wnd_sn);
	item->setTextAlignment(Qt::AlignCenter);
	item->setFlags(item->flags() & (~Qt::ItemIsEditable));	//设置item项不可编辑
	ui.tableWidget->setItem(curRow,UICFG_WND_COL_WND_SN,item);

	item = new QTableWidgetItem(stUW->wnd_name);
	item->setTextAlignment(Qt::AlignVCenter|Qt::AlignLeft);
	//item->setFlags(item->flags() & (~Qt::ItemIsEditable));	//设置item项不可编辑
	ui.tableWidget->setItem(curRow,UICFG_WND_COL_WND_NAME,item);

	item = new QTableWidgetItem(stUW->create_author);
	item->setTextAlignment(Qt::AlignCenter);
	item->setFlags(item->flags() & (~Qt::ItemIsEditable));	//设置item项不可编辑
	ui.tableWidget->setItem(curRow,UICFG_WND_COL_CREATE_AUTHOR,item);

	item = new QTableWidgetItem(stUW->create_time);
	item->setTextAlignment(Qt::AlignCenter);
	item->setFlags(item->flags() & (~Qt::ItemIsEditable));	//设置item项不可编辑
	ui.tableWidget->setItem(curRow,UICFG_WND_COL_CREATE_TIME,item);

	item = new QTableWidgetItem(stUW->modify_author);
	item->setTextAlignment(Qt::AlignCenter);
	item->setFlags(item->flags() & (~Qt::ItemIsEditable));	//设置item项不可编辑
	ui.tableWidget->setItem(curRow,UICFG_WND_COL_MODIFY_AUTHOR,item);

	item = new QTableWidgetItem(stUW->modify_time);
	item->setTextAlignment(Qt::AlignCenter);
	item->setFlags(item->flags() & (~Qt::ItemIsEditable));	//设置item项不可编辑
	ui.tableWidget->setItem(curRow,UICFG_WND_COL_MODIFY_TIME,item);

	//QString strWndType = wndTypeMap[stUW->wnd_type];
	item = new QTableWidgetItem(stUW->wnd_type);
	item->setTextAlignment(Qt::AlignCenter);
	ui.tableWidget->setItem(curRow,UICFG_WND_COL_WND_TYPE,item);
	
	item = new QTableWidgetItem(stUW->svg_file);
	item->setFlags(item->flags() & (~Qt::ItemIsEditable));	//设置item项不可编辑
	ui.tableWidget->setItem(curRow,UICFG_WND_COL_SVG_FILE,item);

	item = new QTableWidgetItem(stUW->dataset);
	item->setTextAlignment(Qt::AlignVCenter|Qt::AlignLeft);
	ui.tableWidget->setItem(curRow,UICFG_WND_COL_DATASET,item);

	item = new QTableWidgetItem(stUW->ds_cond);
	item->setTextAlignment(Qt::AlignCenter);
	ui.tableWidget->setItem(curRow,UICFG_WND_COL_DS_COND,item);

	item = new QTableWidgetItem(stUW->refresh_sec);
	item->setTextAlignment(Qt::AlignCenter);
	ui.tableWidget->setItem(curRow,UICFG_WND_COL_REFRESH_SEC,item);
}
////////////////////  UicfgWndDelegate  /////////////////////////
UicfgWndDelegate::UicfgWndDelegate(QObject *parent,int no)
	: QItemDelegate(parent)
{
	this->parent=static_cast<view_uicfg*>(parent);
	pDb=DB;
	colno = no;
}

UicfgWndDelegate::~UicfgWndDelegate()
{

}

QWidget * UicfgWndDelegate::createEditor(QWidget * parent,const QStyleOptionViewItem & ,const QModelIndex & index) const
{
	switch(index.column())
	{
		case UICFG_WND_COL_WND_TYPE:
		{
			QComboBox *combo = new QComboBox(parent);
			combo->setMinimumHeight(25);
			combo->addItem(tr("SVG图形窗口"));
			combo->addItem(tr("内存库查询窗口"));
			combo->addItem(tr("历史库编辑窗口"));
			combo->addItem(tr("历史库查询窗口"));
			combo->addItem(tr("报表窗口"));
			return combo;
			break;
		}
		case UICFG_WND_COL_DATASET:
			{
				QComboBox* combo = new QComboBox(parent);
				//combo->setFixedHeight(30);
				combo->setMinimumHeight(24);
				combo->addItem("");
				QString strType = this->parent->getItemValue(index.row(), UICFG_WND_COL_WND_TYPE);
				QString strComboText;
				bool bCheckState = this->parent->getCheckBoxStateAndComBoxText(strComboText);
				SString sql;
				if (strType == "报表窗口")
				{
					sql = "select report_no,report_name from t_ssp_report_info where report_name is not null order by report_name";
					
				} else {
					if (bCheckState && strComboText != "全部")
					{
						sql = SString::toFormat("select name,dw_desc from t_ssp_data_window where dstype=2 and dbtype<>1 and cls_name='%s' order by cls_name desc,dw_desc",
							strComboText.toStdString().c_str());
					} else {
						sql = "select name,dw_desc from t_ssp_data_window where dstype=2 and dbtype<>1 and dw_desc is not null order by cls_name desc,dw_desc";
					}
					
				}
				SRecordset rs;
				int ret = DB->Retrieve(sql,rs);
				if(ret < 0)
				{
					//QMessageBox::warning(this,tr("警告"),tr("数据库查询失败[%1]").arg(sql.data()),QMessageBox::Ok);
					return combo;
				}

				for (int i = 0; i < rs.GetRows(); i++)
				{
					combo->addItem(rs.GetValue(i,1).data());
				}

				return combo;
				break;
			}
	}
	
	return NULL;
}

void UicfgWndDelegate::updateEditorGeometry(QWidget *editor,const QStyleOptionViewItem &option,const QModelIndex &) const
{
	if(editor)
	{
		editor->setGeometry(option.rect);
	}
}

void UicfgWndDelegate::setEditorData(QWidget *editor,const QModelIndex &index) const
{
	if(!index.isValid()) 
		return;

	QString text = index.data(Qt::DisplayRole).toString();


	QComboBox * combo = qobject_cast<QComboBox*>(editor);
	if(combo)
		combo->setCurrentIndex(combo->findText(text));
}

void UicfgWndDelegate::setModelData(QWidget *editor,QAbstractItemModel *model,const QModelIndex &index) const
{
	QString text = QString::null;
	QComboBox * combo = qobject_cast<QComboBox*>(editor);
	if(combo) 
		text = combo->itemText(combo->currentIndex());

	if(model) 
		model->setData(index,QVariant(text),Qt::DisplayRole);
}