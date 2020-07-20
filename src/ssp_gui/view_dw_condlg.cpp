#include "view_dw_condlg.h"

view_dw_condlg::view_dw_condlg(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);
	m_table=new QTableWidget(this);
	pDb=DB;
	initTable();
	QVBoxLayout *layout=new QVBoxLayout();
	QHBoxLayout *layoutBtn=new QHBoxLayout();
	serButton=new QPushButton(tr("查询(&Q)"),this);
	QIcon icon;
	icon.addFile(QString::fromUtf8(":/ssp_gui/Resources/search.gif"), QSize(), QIcon::Normal, QIcon::Off);
	serButton->setIcon(icon);
	serButton->setMaximumHeight(24);
	cancelButton=new QPushButton(tr("取消(&C)"),this);
	QIcon icon1;
	icon1.addFile(QString::fromUtf8(":/ssp_gui/Resources/del.gif"), QSize(), QIcon::Normal, QIcon::Off);
	cancelButton->setIcon(icon1);
	cancelButton->setMaximumHeight(24);
	isEmpty=true;
	m_consets.setAutoDelete(false);
	setWindowIcon(icon);
	setWindowTitle(tr("查询"));
	selAllButton=new QPushButton(tr("全选(&A)"),this);
	selAllButton->setMaximumHeight(24);
	calAllButton=new QPushButton(tr("全不选(&L)"),this);
	selAllButton->setMaximumHeight(24);
	layoutBtn->addWidget(selAllButton);
	layoutBtn->addWidget(calAllButton);
	layoutBtn->addStretch();
	layoutBtn->addWidget(serButton);
	layoutBtn->addWidget(cancelButton);

	layout->addWidget(m_table);
	layout->addLayout(layoutBtn);
	setLayout(layout);
	m_iNo=-1;
	isQueryNecessary=false;
	connect(serButton,SIGNAL(clicked()),this,SLOT(onSerButton()));
	connect(cancelButton,SIGNAL(clicked()),this,SLOT(onCancelButton()));
	connect(selAllButton,SIGNAL(clicked()),this,SLOT(onselAllButton()));
	connect(calAllButton,SIGNAL(clicked()),this,SLOT(oncalAllButton()));
	connect(m_table,SIGNAL(itemClicked(QTableWidgetItem *)),this,SLOT(onItemClicked(QTableWidgetItem *)));
}

view_dw_condlg::~view_dw_condlg()
{

}
void view_dw_condlg::initTable()
{
	QStringList header;
	header<<tr("条件名称")<<tr("比较方式")<<tr("条件值");
	m_table->setColumnCount(3);
	m_table->verticalHeader()->hide();
	m_table->setHorizontalHeaderLabels(header);
	//m_table->setAlternatingRowColors(true);
	//m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
	m_table->setSelectionMode(QAbstractItemView::NoSelection);
	//m_table->horizontalHeader()->setResizeMode(QHeaderView::ResizeToContents);
	m_table->setColumnWidth(0,100);
	m_table->setColumnWidth(1,60);
	m_table->setColumnWidth(2,80);
	m_table->horizontalHeader()->setStretchLastSection(true);
	m_table->horizontalHeader()->setAutoScroll(true);
	m_table->verticalHeader()->setAutoScroll(true);
	//SQt::SetTableWidgetStyle(m_table);
}
//根据dw_sn查找对应查询条件并初始化界面
bool view_dw_condlg::refreshTable(int Sno,QString &err)
{
	conMap.clear();
	m_iNo=Sno;
	//表格初始化	
	while(m_table->rowCount()!=0)
		m_table->removeRow(0);
	m_table->clear();
	QStringList header;
	m_table->setColumnCount(3);
	header<<tr("条件名称")<<tr("比较方式")<<tr("条件值");
	m_table->setHorizontalHeaderLabels(header) ;

	SRecordset conTable;
	bool flag=pDb->RetrieveRecordset(SString::toFormat("select dw_sn,con_sn,con_name,cmp_type,input_type,is_option,ref_name,def_value,dyn_where from t_ssp_data_window_condition where dw_sn=%d order by con_sn",m_iNo),conTable);
	if(flag==false)
	{
		//QMessageBox::information(this,tr("警告"),tr("读取数据库的条件信息失败"),QMessageBox::Ok);
		err=tr("读取数据库的条件信息失败");
		return false;
	}
	//提取sql语句的条件号与列名关系
	QString dw_sql=pDb->SelectInto(SString::toFormat("select dw_select from t_ssp_data_window where dw_sn=%d",m_iNo)).data();
	if(dw_sql.contains("where 1=1"))
	{
		dw_sql=dw_sql.split("where 1=1").at(1);
		QStringList conList=dw_sql.split("}");
		for(int i=0;i<conList.size()-1;i++)
		{
			QString conStr=conList.at(i);
			if(conStr.contains("and"))
			{
				conStr=conStr.split("and").at(1);
				bool leftBlank=true;
				int s1=0;
				int s2=0;
				int i1=0;
				int i2=0;
				QString tmpStr;
				for(int j=0;j<conStr.size();j++)
				{
					if(conStr[j]!=' '&&leftBlank==true)
					{
						leftBlank=false;
						s1=j;
					}
					
					if(tmpStr==""&&leftBlank==false&&(conStr[j]=='<'||conStr[j]=='>'||conStr[j]==' '||conStr[j]=='='||conStr[j]=='!'))
					{
						//第一个符号
						s2=j;
						tmpStr=conStr.left(s2);
						tmpStr=tmpStr.mid(s1);
					}
					if(conStr[j]=='@')
					{
						i1=j;
					}
					if(conStr[j]==']')
					{
						i2=j;
						QString tmp=conStr.left(i2);
						tmp=tmp.mid(i1+1);
						int tmpI=tmp.toInt();
						conMap.insert(tmpI,tmpStr);
					}
				}
			}
		}
	}

	
	for(int row=0;row<conTable.GetRows();row++)
	{
		int curRow=m_table->rowCount();
		m_table->insertRow(m_table->rowCount());
		QString dynCon=conTable.GetValue(row,8).data();
		QTableWidgetItem *item=new QTableWidgetItem();
		item->setText(conTable.GetValue(row,2).data());
		item->setCheckState(Qt::Unchecked);
		if(conTable.GetValue(row,5).toInt()==1)
		{
			item->setFlags(item->flags()&~Qt::ItemIsEditable&~Qt::ItemIsUserCheckable);
		}
		else
		{
			item->setCheckState(Qt::Checked);
			item->setFlags(item->flags()&~Qt::ItemIsEditable&~Qt::ItemIsUserCheckable&~Qt::ItemIsEnabled);
			isQueryNecessary=true;
		}
		m_table->setItem(curRow,0,item);
		
		int conType=conTable.GetValue(row,3).toInt();
		int inputType=conTable.GetValue(row,4).toInt();
		//bool bTimeCmp = false;
		if((conType == 5 || conType == 6) && (inputType == 4 || inputType == 5))
		{
			//bTimeCmp = true;
			conType = 0;
		}
		QComboBox *cbo=new QComboBox(this);
		cbo->setFrame(false);
		switch(conType)
		{
		case 0:
			{
				cbo->setEditable(false);
				cbo->setEnabled(false);
				cbo->hide();
			}
			break;
		case 1:
			{
				QStringList str;
				//str<<"="<<"<>"<<"like%"<<"%like"<<"%like%"<<"not like";
				str<<"等于"<<"不等"<<"模糊查询"<<"向后模糊查询"<<"向前模糊查询"<<"否定模糊查询";
				cbo->addItems(str);
			}
			break;
		case 2:
			{
				QStringList str;
				//str<<">"<<">="<<"<"<<"<="<<"="<<"<>";
				str<<"等于"<<"大于"<<"大于等于"<<"小于"<<"小于等于"<<"不等";
				cbo->addItems(str);
			}
			break;
		case 3:
			{
				QStringList str;
				str<<tr("包含")<<tr("不包含");
				cbo->addItems(str);
			}
			break;
		case 4:
			{
				QStringList str;
				str<<tr("包含")<<tr("不包含");
				cbo->addItems(str);
			}
			break;
		case 5:
			{
				QStringList str;
				//str<<">"<<">="<<"<"<<"<="<<"="<<"<>";
				str<<"等于"<<"大于"<<"大于等于"<<"小于"<<"小于等于"<<"不等";
				cbo->addItems(str);
			}
			break;
		case 6:
			{
				QStringList str;
				//str<<">"<<">="<<"<"<<"<="<<"="<<"<>";
				str<<"等于"<<"大于"<<"大于等于"<<"小于"<<"小于等于"<<"不等";
				cbo->addItems(str);
			}
			break;
		case 7:
			{
				QStringList str;
				//str<<">"<<">="<<"<"<<"<="<<"="<<"<>";
				str<<"等于"<<"大于"<<"大于等于"<<"小于"<<"小于等于"<<"不等";
				cbo->addItems(str);
			}
			break;
		}
		QWidget *widget=new QWidget(this);//cbo在多选时不占满窗口
		//widget->setFrame(false);
		widget->setWindowFlags(Qt::FramelessWindowHint);
		QVBoxLayout *layout=new QVBoxLayout(this);
		layout->setMargin(0);
		layout->addStretch();
		layout->addWidget(cbo);
		layout->addStretch();
		widget->setLayout(layout);
		m_table->setCellWidget(curRow,1,widget);
	
		if(inputType==6||inputType==7)//需要读取引用信息
		{
			SRecordset tmp;
			QList<int> lstDynConNo;
			QMapIterator<int,QString>i(conMap);
			while (i.hasNext()) 
			{
				if(dynCon.contains(i.next().value()))
					lstDynConNo.append(i.key());
			}
			SString tmp1=conTable.GetValue(row,5);
			CSsp_Reference *pRef = GET_DW_CFG->SearchReference(conTable.GetValue(row,6));
			if(pRef == NULL)
			{
				err=tr("读取数据窗口失败");
				return false;
			}
			for(int k=0;k<lstDynConNo.size();k++)
			{
				QString replaceStr=QString("{@COND:%1@}").arg(lstDynConNo.at(k));
				dynCon.replace(replaceStr,"\'0\'");
			}
			SRecordset *pRs;
			if(dynCon!="")
				pRs=pRef->GetRecordset(0,dynCon.toStdString().data());
			else
				pRs=pRef->GetRecordset();

			SRecordset &refTable=*pRs;
			if(inputType==6)
			{
				QComboBox *cbo1=new QComboBox(this);
				cbo1->setProperty("row",curRow);//记录行号
				for(int k=0;k<refTable.GetRows();k++)
				{
					cbo1->addItem(refTable.GetValue(k,1).data());
				}
				m_table->setCellWidget(curRow,2,cbo1);
				connect(cbo1,SIGNAL(currentIndexChanged(int)),this,SLOT(dynConditionChanged()));
			}
			else if(inputType==7)
			{
				QTableWidget *tmptable=new QTableWidget(this);//插入一个单列表格
				tmptable->setProperty("row",curRow);//记录行号
				tmptable->setColumnCount(1);
				tmptable->setFrameShape(QFrame::NoFrame);
				tmptable->setFrameStyle(QFrame::NoFrame);
				tmptable->verticalHeader()->hide();
				tmptable->horizontalHeader()->hide();
				tmptable->setSelectionMode(QAbstractItemView::NoSelection);
				tmptable->horizontalHeader()->setStretchLastSection(true);
				tmptable->horizontalHeader()->setAutoScroll(true);
				tmptable->verticalHeader()->setAutoScroll(true);
				

				tmptable->insertRow(tmptable->rowCount());
				QTableWidgetItem *item=new QTableWidgetItem("全部");
				item->setCheckState(Qt::Unchecked);
				item->setFlags(item->flags()&~Qt::ItemIsEditable&~Qt::ItemIsUserCheckable);
				tmptable->setItem(tmptable->rowCount()-1,0,item);
				tmptable->setRowHeight(tmptable->rowCount()-1,18);//行高设为18

				for(int k=0;k<refTable.GetRows();k++)
				{
					tmptable->insertRow(tmptable->rowCount());
					QTableWidgetItem *item=new QTableWidgetItem();
					item->setText(refTable.GetValue(k,1).data());
					item->setCheckState(Qt::Unchecked);
					item->setFlags(item->flags()&~Qt::ItemIsEditable&~Qt::ItemIsUserCheckable);
					tmptable->setItem(tmptable->rowCount()-1,0,item);
					tmptable->setRowHeight(tmptable->rowCount()-1,18);//行高设为18
				}
				
				tmptable->horizontalHeader()->setStretchLastSection(true);
				m_table->setRowHeight(curRow,120);
				m_table->setCellWidget(curRow,2,tmptable);
				
				connect(tmptable,SIGNAL(itemClicked(QTableWidgetItem *)),this,SLOT(onInTableItemClicked(QTableWidgetItem *)));
				connect(tmptable,SIGNAL(itemChanged(QTableWidgetItem *)),this,SLOT(dynConditionChanged()));
			}
		}
		else if(inputType==1||inputType==2||inputType==3)//文本输入
		{
			//QTableWidgetItem *item=new QTableWidgetItem();
			//m_table->setItem(curRow,2,item);
			QLineEdit *item=new QLineEdit();
			item->setFrame(false);
			if(inputType==2)//浮点数
			{
				QRegExp regx("[\\+-]?\\d+(\\.\\d+)?");
				QValidator *validator = new QRegExpValidator(regx,this);
				item->setValidator( validator );
			}
			else if(inputType==3)//整数
			{
				QRegExp regx("^[\\+-]?\\d+$");
				QValidator *validator = new QRegExpValidator(regx,this);
				item->setValidator( validator );
			}
			m_table->setCellWidget(curRow,2,item);
		}
		else if(inputType==4)//日期
		{
			QDateEdit *item=new QDateEdit();
			item->setFrame(false);
			item->setDisplayFormat("yyyy-MM-dd");
			item->setCalendarPopup(true);
			if(conTable.GetValue(row,2).find("结束")>=0)
			{
				SDateTime now = SDateTime::currentDateTime();
				now.addDays(1);
				item->setDate(QDate(now.year(),now.month(),now.day()));
			}
			else
			{
				SDateTime now = SDateTime::currentDateTime();
				item->setDate(QDate(now.year(),now.month(),now.day()));
			}
			if(conTable.GetValue(row,7)!="")
				item->setDate(getDatebyDef(conTable.GetValue(row,7).data()));
			m_table->setCellWidget(curRow,2,item);
		}
		else if(inputType==5)//日期时间
		{
			QDateTimeEdit *item=new QDateTimeEdit();
			item->setFrame(false);
			item->setDisplayFormat("yyyy-MM-dd hh:mm:ss");
			item->setCalendarPopup(true);
			if(conTable.GetValue(row,2).find("结束")>=0)
			{
				SDateTime now = SDateTime::currentDateTime();
				item->setDateTime(QDateTime(QDate(now.year(),now.month(),now.day()),QTime(23,59,59)));
			}
			else
			{
				SDateTime now = SDateTime::currentDateTime();
				item->setDateTime(QDateTime(QDate(now.year(),now.month(),now.day()),QTime(0,0,0)));
			}
			if(conTable.GetValue(row,7)!="")
				item->setDateTime(getDateTimebyDef(conTable.GetValue(row,7).data()));
			m_table->setCellWidget(curRow,2,item);
		}
		isEmpty=false;
	}
	
	return true;
}
void view_dw_condlg::dynConditionChanged()
{
	SRecordset conTable;
	bool flag=pDb->RetrieveRecordset(SString::toFormat("select dw_sn,con_sn,con_name,cmp_type,input_type,is_option,ref_name,def_value,dyn_where from t_ssp_data_window_condition where dw_sn=%d order by con_sn",m_iNo),conTable);

	if(flag==false)
	{
		return;
	}
	if(m_table->rowCount()!=conTable.GetRows())
	{
		QMessageBox::warning(this,"警告","列数不对应,需要检查条件配置是否正确");
		return;
	}
	sender()->blockSignals(true);
	int currentRow=sender()->property("row").toInt();//取得修改行号
	for(int row1=0;row1<conTable.GetRows();row1++)
	{
		if(row1==currentRow)
			continue;
		QString dynCon=conTable.GetValue(row1,8).data();
		if(dynCon=="")
			continue;
		int inputType=conTable.GetValue(row1,4).toInt();
		if(!(inputType==6||inputType==7))
			continue;
		//MEMO: 原逻辑不正确 [2016-3-4 21:08 邵凯田]
		//判断当前条件是否含动态条件
		SString sDynCon = dynCon.toStdString().data();
		while(sDynCon.find("{@COND:")>=0)
		{
			int p1 = sDynCon.find("{@COND:");
			int p2 = sDynCon.find("@}");
			if(p1 <0 || p2 < 0)
				break;
			int cnd = sDynCon.substr(p1+7,p2-p1-7).toInt()-1;
			SString sCndValue;//cnd列对应的值
			{
				CSsp_Reference *pRef = GET_DW_CFG->SearchReference(conTable.GetValue(cnd,6));
				if(pRef == NULL)
				{
					break;
				}
				SRecordset *pRs = pRef->GetRecordset();
				QWidget *w=m_table->cellWidget(cnd,2);
				if(w == NULL)
					break;
				SString text=((QComboBox*)w)->currentText().toStdString().data();
				for(int k=0;k<pRs->GetRows();k++)
				{
					if(pRs->GetValue(k,1)==text)
					{
						sCndValue = pRs->GetValue(k,0);
						break;
					}
				}
			}
			sDynCon = sDynCon.left(p1)+sCndValue+sDynCon.mid(p2+2);
		}
		dynCon = sDynCon.data();
		/*
		QList<int> lstDynConNo;
		QMapIterator<int,QString>i(conMap);
		bool isCurRowAble=false;//修改行是否影响到当前行
		while (i.hasNext()) 
		{
			if(dynCon.contains(i.next().value()))
			{
				lstDynConNo.append(i.key());
				if(i.key()==currentRow+1)
					isCurRowAble=true;//如果当前行动态条件包含修改行行号
			}
		}
		if(isCurRowAble==false)
			continue;
		for(int k=0;k<lstDynConNo.size();k++)
		{
			int row=lstDynConNo.at(k)-1;
			if(row==row1)
				continue;
			int conType=conTable.GetValue(row,3).toInt();	
			int inputType=conTable.GetValue(row,4).toInt();
			QString replaceStr=QString("{@COND:%1@}").arg(lstDynConNo.at(k));

			if(m_table->item(row,0)->checkState()!=Qt::Checked)//未选择的给0值
			{
				dynCon.replace(replaceStr,"\'0\'");
				continue;
			}

			//取得第三列条件
			if(inputType==1)//随意
			{
				QWidget *w=m_table->cellWidget(row,2);
				SString text=((QLineEdit*)w)->text().toStdString().data();

				//MEMO:  [2016-3-4 16:08 邵凯田]QString capStr="\'"+text+"\'";
				QString capStr=text;
				dynCon.replace(replaceStr,capStr);
			}
			else if(inputType==2)//数字
			{
				QWidget *w=m_table->cellWidget(row,2);
				SString text=((QLineEdit*)w)->text().toStdString().data();

				QString capStr="\'"+text+"\'";
				dynCon.replace(replaceStr,capStr);
			}
			else if(inputType==3)//整数
			{
				QWidget *w=m_table->cellWidget(row,2);
				SString text=((QLineEdit*)w)->text().toStdString().data();

				QString capStr="\'"+text+"\'";
				dynCon.replace(replaceStr,capStr);
			}
			else if(inputType==4)//日期
			{
				QWidget *w=m_table->cellWidget(row,2);
				SString time=QString::number(-((QDateEdit*)w)->dateTime().secsTo(QDateTime(QDate(1970,1,1), QTime(0,0)))).toStdString().data();
				SString text=((QDateEdit*)w)->dateTime().toString("yyyy-MM-dd").toStdString().data();

				QString capStr="\'"+text+"\'";
				dynCon.replace(replaceStr,capStr);
			}
			else if(inputType==5)//日期时间
			{
				QWidget *w=m_table->cellWidget(row,2);
				SString time=QString::number(-((QDateEdit*)w)->dateTime().secsTo(QDateTime(QDate(1970,1,1), QTime(0,0)))).toStdString().data();
				SString text=((QDateTimeEdit*)w)->dateTime().toString("yyyy-MM-dd hh:mm:ss").toStdString().data();

				QString capStr="\'"+text+"\'";
				dynCon.replace(replaceStr,capStr);
			}
			else if(inputType==6)//单选
			{
				CSsp_Reference *pRef = GET_DW_CFG->SearchReference(conTable.GetValue(row,6));
				if(pRef == NULL)
				{
					continue;
				}
				SRecordset *pRs = pRef->GetRecordset();
				QWidget *w=m_table->cellWidget(row,2);
				SString text=((QComboBox*)w)->currentText().toStdString().data();

				for(int p=0;p<pRs->GetRows();p++)
				{
					if(pRs->GetValue(p,1)==text)
					{
						QString capStr="\'"+QString(pRs->GetValue(p,0).data())+"\'";
						dynCon.replace(replaceStr,capStr);
						break;
					}
				}
			}
			else if(inputType==7)//多选
			{
				CSsp_Reference *pRef = GET_DW_CFG->SearchReference(conTable.GetValue(row,6));
				if(pRef == NULL)
				{
					continue;
				}
				SRecordset *pRs = pRef->GetRecordset();
				QWidget *w=m_table->cellWidget(row,2);

				QTableWidget *tab=((QTableWidget*)w);
				int rowCount=tab->rowCount();
				QString capStr;
				for(int j=0;j<rowCount;j++)
				{
					SString text=tab->item(j,0)->text().toStdString().data();
					if(text=="全部"&&j==0)
						continue;
					if(tab->item(j,0)->checkState()==Qt::Checked)
					{
						for(int p=0;p<pRs->GetRows();p++)
						{
							if(pRs->GetValue(p,1)==text)
							{
								capStr=capStr+","+"\'"+pRs->GetValue(p,0).data()+"\'";
								break;
							}
						}
					}
				}
				capStr=capStr.mid(1);
				if(capStr=="")
					capStr="\'0\'";
				dynCon.replace(replaceStr,capStr);
			}
			
		}*/
		
		CSsp_Reference *pRef = GET_DW_CFG->SearchReference(conTable.GetValue(row1,6));
		if(pRef == NULL)
		{
			continue;
		}
		SRecordset *pRs;
		pRef->SetInnerRsIsDirty();
		if(dynCon!="")
			pRs=pRef->GetRecordset(0,dynCon.toStdString().data());
		else
			pRs=pRef->GetRecordset();
		//重设条件选择框
		SRecordset &refTable=*pRs;
		if(inputType==6)
		{
 			QWidget *w=m_table->cellWidget(row1,2);
			m_table->removeCellWidget(row1,2);
 			delete w;
			QComboBox *cbo1=new QComboBox(this);
			cbo1->setProperty("row",row1);//记录行号
			for(int k=0;k<refTable.GetRows();k++)
			{
				cbo1->addItem(refTable.GetValue(k,1).data());
			}
			
			m_table->setCellWidget(row1,2,cbo1);
			connect(cbo1,SIGNAL(currentIndexChanged(int)),this,SLOT(dynConditionChanged()));
		}
		else if(inputType==7)
		{
 			QWidget *w=m_table->cellWidget(row1,2);
			//int curRowCount=((QTableWidget*)w)->rowCount();
			m_table->removeCellWidget(row1,2);
 			delete w;
			QTableWidget *tmptable=new QTableWidget(this);//插入一个单列表格
			tmptable->setProperty("row",row1);//记录行号
			tmptable->setColumnCount(1);
			tmptable->setFrameShape(QFrame::NoFrame);
			tmptable->setFrameStyle(QFrame::NoFrame);
			tmptable->verticalHeader()->hide();
			tmptable->horizontalHeader()->hide();
			tmptable->setSelectionMode(QAbstractItemView::NoSelection);
			tmptable->horizontalHeader()->setStretchLastSection(true);
			tmptable->horizontalHeader()->setAutoScroll(true);
			tmptable->verticalHeader()->setAutoScroll(true);

			tmptable->insertRow(tmptable->rowCount());
			QTableWidgetItem *item=new QTableWidgetItem("全部");
			item->setCheckState(Qt::Unchecked);
			item->setFlags(item->flags()&~Qt::ItemIsEditable&~Qt::ItemIsUserCheckable);
			tmptable->setItem(tmptable->rowCount()-1,0,item);
			tmptable->setRowHeight(tmptable->rowCount()-1,18);//行高设为18

			for(int k=0;k<refTable.GetRows();k++)
			{
				tmptable->insertRow(tmptable->rowCount());
				QTableWidgetItem *item=new QTableWidgetItem();
				item->setText(refTable.GetValue(k,1).data());
				item->setCheckState(Qt::Unchecked);
				item->setFlags(item->flags()&~Qt::ItemIsEditable&~Qt::ItemIsUserCheckable);
				tmptable->setItem(tmptable->rowCount()-1,0,item);
				tmptable->setRowHeight(tmptable->rowCount()-1,18);//行高设为18
			}
			tmptable->horizontalHeader()->setStretchLastSection(true);
			m_table->setRowHeight(row1,120);
			m_table->setCellWidget(row1,2,tmptable);
			m_table->update();
			connect(tmptable,SIGNAL(itemClicked(QTableWidgetItem *)),this,SLOT(onInTableItemClicked(QTableWidgetItem *)));
			connect(tmptable,SIGNAL(itemChanged(QTableWidgetItem *)),this,SLOT(dynConditionChanged()));
		}
	}
	sender()->blockSignals(false);
}
//通过槽修改勾选
void view_dw_condlg::onItemClicked(QTableWidgetItem *item)
{
	int col=item->column();
	//int row=item->row();
	if(item->flags()==(item->flags()&~Qt::ItemIsEditable&~Qt::ItemIsUserCheckable&~Qt::ItemIsEnabled))
		return;
	if(col==0)
	{
		if(item->checkState()==Qt::Checked)
			item->setCheckState(Qt::Unchecked);
		else
			item->setCheckState(Qt::Checked);
	}
}
//通过槽修改勾选
void view_dw_condlg::onInTableItemClicked(QTableWidgetItem *item)
{
	int col=item->column();
	int row=item->row();
	if(item->flags()==(item->flags()&~Qt::ItemIsEditable&~Qt::ItemIsUserCheckable&~Qt::ItemIsEnabled))
		return;
	if(col==0)
	{
		if(item->checkState()==Qt::Checked)
			item->setCheckState(Qt::Unchecked);
		else
			item->setCheckState(Qt::Checked);
	}
	if(row==0&&item->text()=="全部")
	{
		QTableWidget *table=(QTableWidget*) sender();
		if(item->checkState()==Qt::Checked)
		{
			for(int i=0;i<table->rowCount();i++)
			{
				table->item(i,0)->setCheckState(Qt::Checked);
			}
		}
		else
		{
			for(int i=0;i<table->rowCount();i++)
			{
				table->item(i,0)->setCheckState(Qt::Unchecked);
			}
		}
	}
}
QDate view_dw_condlg::getDatebyDef(QString defValue)
{
	if(defValue=="TODAY")
		return QDate::currentDate();
	else if(defValue=="YESTERDAY")
		return QDate::currentDate().addDays(-1);
	else if(defValue=="LASTWEEK")
		return QDate::currentDate().addDays(-7);
	else if(defValue=="LASTMONTH")
		return QDate::currentDate().addMonths(-1);
	else if(defValue=="LASTYEAR")
		return QDate::currentDate().addYears(-1);
	return QDate::currentDate();
}
QDateTime view_dw_condlg::getDateTimebyDef(QString defValue)
{
	QDateTime dateTime;
	QTime beginTime(0,0,0);
	QTime endTime(23,59,59);
	dateTime.setDate(QDate::currentDate());
	if(defValue=="TODAY_BEGIN")
	{
		dateTime.setTime(beginTime);
		return dateTime;
	}
	else if(defValue=="TODAY_END")
	{
		dateTime.setTime(endTime);
		return dateTime;
	}
	else if(defValue=="TODAY_NOW")
		return QDateTime::currentDateTime();
	else if(defValue=="YESTERDAY_BEGIN")
	{
		dateTime.setTime(beginTime);
		return dateTime.addDays(-1);
	}
	else if(defValue=="YESTERDAY_END")
	{
		dateTime.setTime(endTime);
		return dateTime.addDays(-1);
	}
	else if(defValue=="YESTERDAY_NOW")
		return QDateTime::currentDateTime().addDays(-1);
	return QDateTime::currentDateTime();
}

bool view_dw_condlg::refreshTableByName(SString name,QString &err)
{
	SRecordset tmpSets;
	bool flag=pDb->RetrieveRecordset(SString::toFormat("select dw_sn from t_ssp_data_window where name='%s'",name.data()),tmpSets);
	if(flag==false)
	{
		//QMessageBox::information(this,tr("警告"),tr("初始化动态查询条件窗口时读取数据库失败"),QMessageBox::Ok);
		err=tr("读取数据库失败");
		return false;
	}
	if(!refreshTable(tmpSets.GetValue(0,0).toInt(),err))
		return false;
	return true;
}
SPtrList<stuDwCondition> *view_dw_condlg::getListDwCon()
{
	return &m_consets;
}
//是否按下确定？
bool view_dw_condlg::isOk()
{
	return isOkey;
}
//是否存在必须条件？
bool view_dw_condlg::getisQueryNecessary()
{
	return isQueryNecessary;
}
//是否是空条件？
bool view_dw_condlg::isConditionEmpty()
{
	return isEmpty;
}
//全选
void view_dw_condlg::onselAllButton()
{
	int rowCount=m_table->rowCount();
	for(int i=0;i<rowCount;i++)
	{
		m_table->item(i,0)->setCheckState(Qt::Checked);
	}
}
//全反选
void view_dw_condlg::oncalAllButton()
{
	int rowCount=m_table->rowCount();
	for(int i=0;i<rowCount;i++)
	{
		if(m_table->item(i,0)->flags()!=(m_table->item(i,0)->flags()&~Qt::ItemIsEditable&~Qt::ItemIsUserCheckable&~Qt::ItemIsEnabled))
		{
			m_table->item(i,0)->setCheckState(Qt::Unchecked);
		}	
	}
}
//查询
void view_dw_condlg::onSerButton()
{
	conStr="";
	SRecordset conTable;
	bool flag=pDb->RetrieveRecordset(SString::toFormat("select dw_sn,con_sn,con_name,cmp_type,input_type,is_option,ref_name,def_value,dyn_where from t_ssp_data_window_condition where dw_sn=%d order by con_sn",m_iNo),conTable);

	if(flag==false)
	{
		hide();
		return;
	}
	if(m_table->rowCount()!=conTable.GetRows())
	{
		hide();
		return;
	}
	m_consets.clear();
	for(int row=0;row<conTable.GetRows();row++)
	{
		stuDwCondition *stucon=new stuDwCondition();
		int conType=conTable.GetValue(row,3).toInt();	
		int inputType=conTable.GetValue(row,4).toInt();
		bool bTimeCmp = false;
		if((conType == 5 || conType == 6) && (inputType == 4 || inputType == 5))
		{
			bTimeCmp = true;
			conType = 0;
		}
		stucon->iConditionSn=row+1;
		if(m_table->item(row,0)->checkState()!=Qt::Checked)
		{
			delete stucon;
			continue;
		}
		conStr=conStr+QString(conTable.GetValue(row,2).data())+" ";
		int islike=0;
		//1	
		SString capValue;
		//2
		QString cboText=((QComboBox*)m_table->cellWidget(row,1)->layout()->itemAt(1)->widget())->currentText();
		conStr=conStr+cboText+" ";
		if(cboText!="")
		{
			if(cboText==tr("包含"))
				capValue=capValue+"in";
			else if(cboText==tr("不包含"))
				capValue=capValue+"not in";
			else if(cboText==tr("模糊查询"))
			{
				capValue=capValue+"like";
				islike=1;
			}
			else if(cboText==tr("向前模糊查询"))
			{
				capValue=capValue+"like";
				islike=2;
			}
			else if(cboText==tr("向后模糊查询"))
			{
				capValue=capValue+"like";
				islike=3;
			}
			else if(cboText==tr("否定模糊查询"))
			{
				capValue=capValue+"not like";
				islike=4;
			}
			else if(cboText==tr("大于"))
			{
				capValue=capValue+">";
			}
			else if(cboText==tr("大于等于"))
			{
				capValue=capValue+">=";
			}
			else if(cboText==tr("小于"))
			{
				capValue=capValue+"<";
			}
			else if(cboText==tr("小于等于"))
			{
				capValue=capValue+"<=";
			}
			else if(cboText==tr("等于"))
			{
				capValue=capValue+"=";
			}
			else if(cboText==tr("不等"))
			{
				capValue=capValue+"<>";
			}
			else 
			{
				capValue=capValue+cboText.toStdString().data();
			}
		}
		//3
		if(inputType==1)//随意
		{
			QWidget *w=m_table->cellWidget(row,2);
			SString text=((QLineEdit*)w)->text().toStdString().data();
			if(text=="")//空行
			{
				QMessageBox::information(this,tr("警告"),tr("存在条件为空"),QMessageBox::Ok);
				m_consets.clear();
				delete stucon;
				return;
			}
			conStr=conStr+QString(text.data())+" ";
			if(islike==1)
				capValue=capValue+"\'%"+text+"%\'";
			else if(islike==2)
				capValue=capValue+"\'%"+text+"\'";
			else if(islike==3)
				capValue=capValue+"\'"+text+"%\'";
			else if(islike==4)
				capValue=capValue+"\'%"+text+"%\'";
			else
				//MEMO:  [2016-3-4 16:16 邵凯田]capValue=capValue+"\'"+text+"\'";
				capValue=capValue+text;
		}
		else if(inputType==2)//数字
		{
			QWidget *w=m_table->cellWidget(row,2);
			SString text=((QLineEdit*)w)->text().toStdString().data();
			conStr=conStr+QString(text.data())+" ";
			if(text=="")//空行
			{
				QMessageBox::information(this,tr("警告"),tr("存在条件为空"),QMessageBox::Ok);
				m_consets.clear();
				delete stucon;
				return;
			}
			capValue=capValue+text;
		}
		else if(inputType==3)//整数
		{
			QWidget *w=m_table->cellWidget(row,2);
			SString text=((QLineEdit*)w)->text().toStdString().data();
			conStr=conStr+QString(text.data())+" ";
			if(text=="")//空行
			{
				QMessageBox::information(this,tr("警告"),tr("存在条件为空"),QMessageBox::Ok);
				m_consets.clear();
				delete stucon;
				return;
			}
			capValue=capValue+text;
		}
		else if(inputType==4)//日期
		{
			QWidget *w=m_table->cellWidget(row,2);
			SString time;//=QString::number(-((QDateEdit*)w)->dateTime().secsTo(QDateTime(QDate(1970,1,1), QTime(0,0)))).toStdString().data();
			QDateTime dt = ((QDateEdit*)w)->dateTime();
			SString text=dt.toString("yyyy-MM-dd").toStdString().data();
			if(bTimeCmp)
				time = text;
			else
				time.sprintf("%d",SDateTime::makeDateTime("yyyy-MM-dd",text).soc());
			conStr=conStr+QString(text.data())+" ";
			capValue=capValue+time;
		
		}
		else if(inputType==5)//日期时间
		{
			QWidget *w=m_table->cellWidget(row,2);
			SString time;//=QString::number(-((QDateEdit*)w)->dateTime().secsTo(QDateTime(QDate(1970,1,1), QTime(0,0)))).toStdString().data();
			QDateTime dt = ((QDateTimeEdit*)w)->dateTime();
			SString text=dt.toString("yyyy-MM-dd hh:mm:ss").toStdString().data();
			if(bTimeCmp)
				time = text;
			else
				time.sprintf("%d",SDateTime::makeDateTime("yyyy-MM-dd hh:mm:ss",text).soc());
			conStr=conStr+QString(text.data())+" ";
			capValue=capValue+time;
		}
		else if(inputType==6)//单选
		{
			CSsp_Reference *pRef = GET_DW_CFG->SearchReference(conTable.GetValue(row,6));
			if(pRef == NULL)
			{
				delete stucon;
				return;
			}
			SRecordset *pRs = pRef->GetInnerRecordsetDirect();
			QWidget *w=m_table->cellWidget(row,2);
			SString text=((QComboBox*)w)->currentText().toStdString().data();
			conStr=conStr+QString(text.data())+" ";
			for(int k=0;k<pRs->GetRows();k++)
			{
				if(pRs->GetValue(k,1)==text)
				{
					capValue=capValue+"(\'"+pRs->GetValue(k,0)+"\')";
					break;
				}
			}
		}
		else if(inputType==7)//多选
		{
			CSsp_Reference *pRef = GET_DW_CFG->SearchReference(conTable.GetValue(row,6));
			if(pRef == NULL)
			{
				delete stucon;
				return;
			}
			SRecordset *pRs = pRef->GetInnerRecordsetDirect();
			QWidget *w=m_table->cellWidget(row,2);
			
			QTableWidget *tab=((QTableWidget*)w);
			int rowCount=tab->rowCount();
			capValue=capValue+"(";
			for(int j=0;j<rowCount;j++)
			{
				if(tab->item(j,0)->checkState()==Qt::Checked)
				{
					SString text=tab->item(j,0)->text().toStdString().data();
					if(text=="全部"&&j==0)
						continue;
					conStr=conStr+QString(text.data())+" ";
					for(int k=0;k<pRs->GetRows();k++)
					{
						if(pRs->GetValue(k,1)==text)
						{
							capValue=capValue+"\'"+pRs->GetValue(k,0)+"\',";
							break;
						}
					}
				}
			}
			capValue=capValue.substr(0,capValue.length()-1)+")";
			if(capValue=="in)"||capValue=="not in)")//一个没选不存
			{
				QMessageBox::information(this,tr("警告"),tr("多选条件存在条件为空"),QMessageBox::Ok);
				m_consets.clear();
				delete stucon;
				return;
			}
		}
		conStr=conStr+";";
		stucon->sCmpValue=capValue;
		m_consets.append(stucon);
	}
	isOkey=true;
	hide();
}
void view_dw_condlg::onCancelButton()
{
// 	if(isQueryNecessary==true)
// 	{
// 		QMessageBox::information(this,tr("警告"),tr("存在必选查询，不可以取消"),QMessageBox::Ok);
// 		return;
// 	}
	isOkey=false;
	hide();
}
void view_dw_condlg::closeEvent(QCloseEvent *event)
{
	isOkey=false;
	return QDialog::closeEvent(event);
}