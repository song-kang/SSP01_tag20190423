#include "sys_mdbgui.h"

sys_mdbgui::sys_mdbgui(QWidget *parent, Qt::WFlags flags)
	: QMainWindow(parent, flags)
{
	ui.setupUi(this);
	this->setWindowState(Qt::WindowMaximized);
	r=NULL;
	citem = NULL;
	l_reg = 0;
	l_select = 0;
	l_operate = 0;
	b_connect = false;
	m_index = -1;
	r = new connectMenu(this,Mmenu::NOITEM);
	r->regist(this);
	b_edit = true;
	ui.menu->addAction(r->aConnect);
	ui.menu->addAction(r->aDelDb);
	ui.menu->addAction(r->aDelTable);
	ui.treeWidget->headerItem()->setText(0,QString::fromLocal8Bit("连接终端"));
	qRegisterMetaType<sendInfo>("sendInfo");
	m_SendInfo.setAutoDelete(true);
	m_SendInfo.setShared(true);
	QObject::connect(ui.tableWidget,SIGNAL(itemChanged(QTableWidgetItem *)),this,SLOT(updateData(QTableWidgetItem *)));
	//QObject::connect(this,SIGNAL(setInfo()),this,SLOT(setRegistInfo()));
	QObject::connect(dbinter.getRsignal(),SIGNAL(runsql(SString,bool)),this,SLOT(RecordHistory(SString,bool)));
	QObject::connect(dbinter.getRsignal(),SIGNAL(updateTree(SString,UPDATETYPE)),this,SLOT(reloadDb(SString,UPDATETYPE)));
	QPalette pal;
	pal.setColor(QPalette::Base, QColor(255, 255, 255));
	pal.setColor(QPalette::AlternateBase, QColor(10, 10, 10,40));
	ui.tableWidget->setPalette(pal);
	ui.tableWidget->setAlternatingRowColors(true);
	ui.pushButton_del->setEnabled(false);
	ui.pushButton_save->setEnabled(false);
	ui.pushButton_insert->setEnabled(false);
	this->setWindowTitle(QString::fromLocal8Bit("内存数据库管理工具"));
	this->setWindowIcon(QIcon(":/Resources/icon/exe.png"));
	b_first = true;
	setShortCut();
	connect();
	b_append=true;
	m_timer=new QTimer(this);
	QObject::connect(m_timer, SIGNAL(timeout()), this, SLOT(setRegistInfo()));
	m_vfields.clear();
	m_timer->start(1);
	m_blocal=false;
}
 
sys_mdbgui::~sys_mdbgui()
{
	b_append=false;
	delete r;
	removeAllRegistCall();
	m_SendInfo.clear();
	for(int i=0;i<m_vfields.size();i++)
	{
		
		if (m_vfields[i].lf!=NULL)
		{
			delete m_vfields[i].lf;
			m_vfields[i].lf=NULL;
		}
	}

}
void sys_mdbgui::reloadDb(SString sTable,UPDATETYPE type)
{
	if (b_first)
	{
		return;
	}
	QTreeWidgetItem *m_topItem;
	m_topItem= ui.treeWidget->topLevelItem(m_index);
	int i_count = m_topItem->childCount();
	QTreeWidgetItem *citm;
	switch(type)
	{
	case CREATE:

		citm = new QTreeWidgetItem(m_topItem);
		citm->setText(0,sTable.data());
		break;
	case DROP:
		for(int i = 0;i<i_count;i++)
		{
			if (sTable==m_topItem->child(i)->text(0))
			{
				m_topItem->takeChild(i);
				i_count--;
			}
		}
		break;
	case SELECTALL:
		ui.tableWidget->setEditTriggers(QAbstractItemView::DoubleClicked);
		m_curTableName = sTable.data();
		break;
	}
}
void sys_mdbgui::reloadDb()
{
	QTreeWidgetItem *m_topItem;
	m_topItem= ui.treeWidget->topLevelItem(m_index);
	m_topItem->takeChildren();
	dbinter.execSql(SString("select * from systable").stripWhiteSpace());
	for (int i = 0;i<dbinter.getRow();i++)
	{
		QTreeWidgetItem *citm = new QTreeWidgetItem(m_topItem);
		citm->setText(0,dbinter.getValue(i,0).data());
		ui.treeWidget->addTopLevelItem(m_topItem);
	}
	m_topItem->setExpanded(true);
}
void sys_mdbgui::refresh(SString s_table)
{
	
	if (s_table==m_curTableName)
	{
		on_treeWidget_itemDoubleClicked(citem,0);
	}
}
void sys_mdbgui::RecordHistory(SString sSql,bool b_select)
{
	if (l_select>MAX_SHOW_SIZE)
	{
		ui.textEdit_3->clear();
		l_select = 0;
	}
	if (l_operate>MAX_SHOW_SIZE)
	{
		ui.textEdit_4->clear();
		l_operate = 0;
	}
	QString s_info;
	SString sysDate = QDate::currentDate().toString("yyyy-MM-dd").toStdString();
	SString sysTime = QTime::currentTime().toString("hh:mm:ss").toStdString();
	SString s_result =sysDate + " "+ sysTime+":执行 "+sSql + "\r\n";
	s_info = QString::fromLocal8Bit(s_result.data());
	if (b_select)
	{
		QTextCursor Qcur= ui.textEdit_3->textCursor();
		Qcur.movePosition(QTextCursor::End);
		ui.textEdit_3->setTextCursor(Qcur);
		ui.textEdit_3->insertPlainText(s_info);
		l_select+=sSql.size();
		QScrollBar *v_bar = ui.textEdit_3->verticalScrollBar();
		if (v_bar!=NULL)
		{
			v_bar->setValue(v_bar->maximumHeight());
		}
	}else
	{
		QTextCursor Qcur= ui.textEdit_4->textCursor();
		Qcur.movePosition(QTextCursor::End);
		ui.textEdit_4->setTextCursor(Qcur);
		ui.textEdit_4->insertPlainText(s_info);
		l_operate+=sSql.size();
		QScrollBar *v_bar = ui.textEdit_4->verticalScrollBar();
		if (v_bar!=NULL)
		{
			v_bar->setValue(v_bar->maximumHeight());
		}
	}

}
void sys_mdbgui::tableInit()
{
	ui.tableWidget->clear();
	ui.tableWidget->setColumnCount(0);
	ui.tableWidget->setRowCount(0);
	ui.label->clear();
}
void sys_mdbgui::getValue(int m_value)
{
	ui.tableWidget->clear();
	int i_colum = ui.tableWidget->columnCount();
	for (int i = 0;i<m_TabPageRowN;i++)
	{
		for (int j=0;j<i_colum;j++)
		{
			ui.tableWidget->setItem(m_value+i,j+1,new QTableWidgetItem(V_item[(m_value+i)*i_colum+j]));
			//ui.tableWidget->setItem(0,0,/*V_item[(m_value+i)*i_colum+j]*/new QTableWidgetItem("asdd"));

		}
	}
}
void sys_mdbgui::on_treeWidget_customContextMenuRequested(QPoint pos)
 {
	 QTreeWidgetItem * qi = NULL;
	 	qi =  ui.treeWidget->itemAt(pos);
		if(qi!=NULL)
			citem = ui.treeWidget->itemAt(pos);
	//释放原先的菜单
	 //if (r!=NULL)
	 //{
		// delete r;
		// r =NULL;
	 //}
	 //判断菜单的种类
	 if (qi!=NULL)
	 {
		 if(/*qi->childCount()==0&&*/qi->parent()!=NULL)
		 {
			 //r = new connectMenu(this,connectMenu::CHILDITEM);

			 //r->regist(this);
			 r->setType(Mmenu::CHILDITEM);
			 r->exec(QCursor::pos());
		 }else
		 {
			 int i_index = 0;
			 i_index = ui.treeWidget->indexOfTopLevelItem(qi->parent());
			 r->setType(Mmenu::FATHERITEM);
			 //r = new connectMenu(this,connectMenu::FATHERITEM);
			 //r->regist(this);
			 r->exec(QCursor::pos());
		 }

	 }else
	 {
		 r->setType(Mmenu::NOITEM);
		 //r = new connectMenu(this,connectMenu::NOITEM);
		 //r->regist(this);
		 r->exec(QCursor::pos());
	 }


 }
void sys_mdbgui::on_treeWidget_itemDoubleClicked( QTreeWidgetItem * item, int column )
{
	isSave();
	b_edit = false;
	int i = ui.treeWidget->indexOfTopLevelItem(item->parent());
	if (i==-1)
	{
		return;
	}
	dbinter.pointTo(i);
	m_index = i;
	QStringList ls;
	m_curTableName = item->text(column);
	if (m_curTableName.contains('*'))
	{
		m_curTableName.remove('*');
	}
	execSqltoWidget("select * from "+m_curTableName);

	if (m_tableMap.count(m_curTableName)==0)
	{
		tableTofield tf;
		tf.stable=m_curTableName.toStdString().data();
		tf.lf=new SPtrList<stuTableField>;
		dbinter.getTablefields(tf.stable,*tf.lf);
		m_vfields.push_back(tf);
		m_tableMap.insert(m_curTableName,m_vfields.size()-1);
	}

	ui.tableWidget->setEditTriggers(QAbstractItemView::DoubleClicked);
	ui.pushButton_insert->setEnabled(true);
	ui.pushButton_del->setEnabled(false);
	ui.pushButton_save->setEnabled(false);
	b_edit = true;
}
void sys_mdbgui::on_action_triggered ( bool checked)
{
	b_edit = false;
	QString m_sql;
	//if (dbinter.manageSql(m_sql.toStdString()))
	//{
	//	return;
	//}
	if (citem==NULL)
	{
		ui.label->setText(QString::fromLocal8Bit("请先选中一个数据库！"));
		return;
	}
	int i = ui.treeWidget->indexOfTopLevelItem(citem->parent());

	if (i==-1)
	{
		i = ui.treeWidget->indexOfTopLevelItem(citem);
	}
	m_sql = ui.textEdit->toPlainText().toLocal8Bit();
		dbinter.pointTo(i);
		execSqltoWidget(m_sql);
		//if (m_sql.trimmed().left(13).toLower()=="select * from"&&m_sql.trimmed().left(22).toLower()!="select * from systable")
		//{
		//	ui.tableWidget->setEditTriggers(QAbstractItemView::DoubleClicked);
		//}
		//reloadDb();
		b_edit = true;
		ui.pushButton_del->setEnabled(false);
		ui.pushButton_insert->setEnabled(false);
		ui.pushButton_save->setEnabled(false);
}
void sys_mdbgui::on_action_runfile_triggered(bool checked)
{
	QStringList filters;
	filters << "Sql files (*.sql)";
	QFileDialog *dialog = new QFileDialog(this);
	dialog->setNameFilters(filters);
		QString qName;
		if (	dialog->exec())
		{
			qName = dialog->selectedFiles().first();
			if (!dbinter.connect())
			{
				return;
			}
			dbinter.RunSqlFile(qName.toStdString());
			ui.label->setText(QString::fromLocal8Bit(dbinter.sRet.data()));
		}
}
void sys_mdbgui::on_action_5_triggered ( bool checked)
{
	QDialog *Qexp = new QDialog();
	Qexp->setWindowTitle(QString::fromLocal8Bit("工具说明"));
	Qexp->width();
	Qexp->height();
	Qexp->setMaximumHeight(170);
	Qexp->setMinimumHeight(170);
	Qexp->setMaximumWidth(330);
	Qexp->setMinimumWidth(330);
	QLabel *ql = new QLabel(Qexp);
	ql->setGeometry(10,10,300,150);
	ql->setAlignment(Qt::AlignCenter); 
	QString text = QString::fromLocal8Bit("内存数据库访问工具");
	ql->setText(text);
	Qexp->exec();
}
void sys_mdbgui::on_action_7_triggered ( bool checked)
{
	QDialog *help = new QDialog();
	help->setWindowTitle(QString::fromLocal8Bit("工具帮助"));
	help->width();
	help->height();
	help->setMaximumHeight(400);
	help->setMinimumHeight(400);
	help->setMaximumWidth(600);
	help->setMinimumWidth(600);
	QLabel *ql = new QLabel(help);
	ql->setGeometry(10,10,580,380);
	ql->setAlignment(Qt::AlignCenter); 
	QString text;
	QFile file("instruction.txt");  
	if ( file.exists())  
	{  
		file.open(QIODevice::ReadOnly);
		text = QTextCodec::codecForName("GBK")->toUnicode(file.readAll().data());
		
	}else
	{
		file.open( QIODevice::WriteOnly );
		file.close(); 
	}



	ql->setText(text);
	help->exec();
}
void sys_mdbgui::on_action_6_triggered ( bool checked)
{
	ui.textEdit_2->clear();
}
void sys_mdbgui::on_action_8_triggered ( bool checked)
{
	ui.textEdit_3->clear();
}
void sys_mdbgui::on_action_9_triggered ( bool checked)
{
	ui.textEdit_4->clear();
}
void sys_mdbgui::on_pushButton_insert_pressed()
{
	if (citem==NULL||citem->childCount()!=0)
	{
		return;
	}
	b_edit=false;
	int m_row = ui.tableWidget->rowCount();
	int m_colum = ui.tableWidget->columnCount();
	ui.tableWidget->setRowCount(m_row);
	ui.tableWidget->insertRow(m_row);
	for (int j = 0;j<m_colum;j++)
	{
		ui.tableWidget->setItem(m_row,j,new QTableWidgetItem());
	}
	dbinter.addcmd("",0,0,"",-1,"");
	ui.pushButton_save->setEnabled(true);
	QScrollBar *v_bar = ui.tableWidget->verticalScrollBar();
	if (v_bar!=NULL)
	{
		v_bar->setValue(v_bar->maximumHeight());
	}
	b_edit=true;
}
void sys_mdbgui::on_pushButton_save_pressed()
{
	m_blocal=true;
	if (!dbinter.isCmdExec())
	{

			int m_row = ui.tableWidget->rowCount();
			int m_colum = ui.tableWidget->columnCount();
			SString s_value;
			for (int i = m_curRow;i<m_row;i++)
			{
				for (int j = 0;j<m_colum;j++)
				{
					s_value+=ui.tableWidget->item(i,j)->text().toLocal8Bit().data();
					s_value+=",";
				}
				dbinter.addcmd(m_curTableName.toStdString(),i,m_colum,s_value,0,s_value);
				s_value.clear();
			}
			dbinter.runRecord();
			ui.pushButton_del->setEnabled(false);
			ui.pushButton_save->setEnabled(false);
			m_curRow = m_row;
	}
	m_blocal=false;
}
void sys_mdbgui::on_pushButton_del_pressed()
{
	SString s_value;
	int m_colum = ui.tableWidget->columnCount();
	QList<QTableWidgetSelectionRange> lSelectedRange;
	lSelectedRange = ui.tableWidget->selectedRanges();
	int i_size = lSelectedRange.size();
	if (lSelectedRange.size()>1)
	{
		for (int i = 0;i<=(i_size-1)/2;i++)
		{
			lSelectedRange.swap(i,i_size-1-i);
		}
	}
	foreach (const QTableWidgetSelectionRange &qRange, lSelectedRange)
	{
		if (qRange.leftColumn()==0&&qRange.rightColumn()==m_colum-1)//选中了整行
		{
			for(int i = qRange.topRow();i<=qRange.bottomRow();i++)
			{
				for(int j = 0;j<m_colum;j++)
				{
					s_value+=(ui.tableWidget->item(i,j))->text().toLocal8Bit().data();
					s_value+=",";
				}
				if (i<m_curRow)
				{
					dbinter.addcmd(m_curTableName.toStdString(),i,m_colum,s_value,1,s_value);	
				}
				s_value.clear();
				
			}
			for(int i = qRange.bottomRow();i>=qRange.topRow();i--)
			{
				ui.tableWidget->removeRow(i);
				if (i<m_curRow)
				{
					m_curRow--;
				}
			}
		}else
		{
			for(int i = qRange.topRow();i<=qRange.bottomRow();i++)
			{
				for(int j = qRange.leftColumn();j<=qRange.rightColumn();j++)
				{
					for(int k = 0;k<m_colum;k++)
					{
						if (j==k)
						{
							s_value+="";
						}		else
						{
							s_value+=(ui.tableWidget->item(i,k))->text().toLocal8Bit().data();				
						}
						s_value+=",";
					}
					dbinter.addcmd(citem->text(0).toStdString(),i,m_colum,s_value,2,s_value);
					s_value.clear();
					ui.tableWidget->item(i,j)->setText("");
				}
			}

		}

	}
	ui.pushButton_save->setEnabled(true);
	ui.pushButton_del->setEnabled(false);
}
void sys_mdbgui::on_tableWidget_itemSelectionChanged()
{
	QList<QTableWidgetSelectionRange> lSelectedRange;
	lSelectedRange = ui.tableWidget->selectedRanges();
	int m_colum = ui.tableWidget->columnCount();
	ui.pushButton_del->setEnabled(false);
	foreach (const QTableWidgetSelectionRange &qRange, lSelectedRange)
	{
		if (qRange.leftColumn()==0&&qRange.rightColumn()==m_colum-1)//选中了整行
		{
			ui.pushButton_del->setEnabled(true);
		}else
		{
			ui.pushButton_del->setEnabled(false);
		}
	}
}
 void sys_mdbgui::connect()
 {
	 if (b_first)
	 {
		 SString s_param = readSeting();
		 getData(s_param);
	 }else
	 {
		 enterDlg e;
		 QObject::connect(&e,SIGNAL(ok_pressed(SString)),this,SLOT(getData(SString)));
		 int x = e.exec();
		 if (x==-1)
		 {
			 return;
		 }
	 }
	 b_first = true;
	 SRecordset rs;
	 if (!b_connect)
	 {
		 ui.label->setText(QString::fromLocal8Bit(dbinter.sRet.data()));
		 b_first = false;
		 return;
	 }
	 m_index = dbinter.getSize()-1;
	 //dbinter.add();
	 dbinter.execSql(SString("select * from systable").stripWhiteSpace());
	 QTreeWidgetItem *topitm = new QTreeWidgetItem(ui.treeWidget);
	 topitm->setText(0,s_ip.data());
	 	 for (int i = 0;i<dbinter.getRow();i++)
	 	 {
	 		 QTreeWidgetItem *citm = new QTreeWidgetItem(topitm);
	 		 citm->setText(0,dbinter.getValue(i,0).data());
	 		 ui.treeWidget->addTopLevelItem(topitm);
	 	 }
		 topitm->setExpanded(true);
		 ui.label->setText(QString::fromLocal8Bit(dbinter.sRet.data()));
		 b_first = false;

 }
 void sys_mdbgui::delDb()
 {
	 if (citem==NULL)
	 {
		 return;
	 }
	 int i = ui.treeWidget->indexOfTopLevelItem(citem);
	 if (citem->childCount()!=0||citem->parent()==NULL)
	 {
		 int i_ret = QMessageBox::warning(this,QString::fromLocal8Bit("警告"),QString::fromLocal8Bit("是否断开该连接？"),QMessageBox::Yes,QMessageBox::No);
		 if (i_ret==QMessageBox::Yes)
		 {
			 ui.treeWidget->takeTopLevelItem(i);
			 dbinter.del(i);
			 if (ui.treeWidget->topLevelItemCount()==0)
			 {
				 tableInit();
			 }
		 }else
		 {
			 return;
		 }
	 }
 }
 void sys_mdbgui::delTable()
 {
	 int i = ui.treeWidget->indexOfTopLevelItem(citem);
	 int i_child = citem->parent()->indexOfChild(citem);
	 if (citem->childCount()==0)
	 {
		 int i_ret = QMessageBox::warning(this,QString::fromLocal8Bit("警告"),QString::fromLocal8Bit("是否删除该表？"),QMessageBox::Yes,QMessageBox::No);
		 if (i_ret==QMessageBox::Yes)
		 {
			 removeRegistCall();
			 dbinter.delTable(citem->text(0).toStdString());
			 //citem->parent()->takeChild(i_child);
			 ui.label->setText(QString::fromLocal8Bit(dbinter.sRet.data()));
		 }else
		 {
			 return;
		 }

	 }
 }
 void sys_mdbgui::registCallback()
 {
	 if (citem==NULL)
	 {
		 return;
	 }
	 if (citem->foreground(0)!=QBrush(QColor(255,0,0)))
	 {
		 QString qTableName = citem->text(0);
		 citem->setText(0,citem->text(0)+"*");
		 citem->setForeground(0,QBrush(QColor(255,0,0)));
		 dbinter.registCallback(registFunction,this,qTableName.toStdString(),0x0d|0x0e|0x0f);
	 }
	 int i=0;

 }
 void sys_mdbgui::removeRegistCall()
 {
	 if (citem==NULL)
	 {
		 return;
	 }
	 if (citem->foreground(0)==QBrush(QColor(255,0,0)))
	 {
		 citem->setText(0,citem->text(0).remove('*'));
		 QString qTableName = citem->text(0);
		citem->setForeground(0,QBrush(QColor(0,0,0)));
		dbinter.removeCallback(registFunction,this,qTableName.toStdString(),0x0f|0x0e|0x0f);
	 }
 }
 void sys_mdbgui::registAllCallback()
 {
	 if (citem==NULL)
	 {
		 return;
	 }
	 QString qTableName;
	 for (int i = 0;i<citem->childCount();i++)
	 {
		 if (citem->child(i)->foreground(0)!=QBrush(QColor(255,0,0)))
		 {
			 qTableName = citem->child(i)->text(0);
			 if (m_tableMap.count(qTableName)==0)
			 {
				 tableTofield tf;
				 tf.stable=qTableName.toStdString().data();
				 tf.lf=new SPtrList<stuTableField>;
				 dbinter.getTablefields(tf.stable,*tf.lf);
				 m_vfields.push_back(tf);
				 m_tableMap.insert(qTableName,m_vfields.size()-1);
			 }
		 }

	 }
	 for (int i = 0;i<citem->childCount();i++)
	 {
		 if (citem->child(i)->foreground(0)!=QBrush(QColor(255,0,0)))
		 {
			 qTableName = citem->child(i)->text(0);
			 citem->child(i)->setText(0,citem->child(i)->text(0)+"*");
			 dbinter.registCallback(registFunction,this,qTableName.toStdString(),0x0d|0x0e|0x0f);
			 citem->child(i)->setForeground(0,QBrush(QColor(255,0,0)));
		 }

	 }
 }
 void sys_mdbgui::removeAllRegistCall()
 {
	 if (citem==NULL)
	 {
		 return;
	 }
	 for (int i = 0;i<citem->childCount();i++)
	 {
		 if (citem->child(i)->foreground(0)==QBrush(QColor(255,0,0)))
		 {
			 citem->child(i)->setText(0,citem->child(i)->text(0).remove('*'));
			 QString qTableName = citem->child(i)->text(0);
			 dbinter.removeCallback(registFunction,this,qTableName.toStdString(),0x0d|0x0e|0x0f);
			 citem->child(i)->setForeground(0,QBrush(QColor(0,0,0)));
		 }

	 }
 }
 void sys_mdbgui::getData(SString sParam)
 {
	 dbinter.create(sParam);
	 QString s_port;
	 QString s_user;
	 QString s_password;
	 if(dbinter.connect())
	 {
		s_ip = SString::GetIdAttribute(2,SString::GetIdAttribute(1,sParam),"=");
		s_port=SString::GetIdAttribute(2,SString::GetIdAttribute(2,sParam),"=");
		s_user=SString::GetIdAttribute(2,SString::GetIdAttribute(3,sParam),"=");
		s_password=SString::GetIdAttribute(2,SString::GetIdAttribute(4,sParam),"=");
		b_connect = true;
		writeSeting(s_ip.data(),s_port,s_user,s_password);
	 }else
	 {
		 b_connect = false;
	 }
 }
 void sys_mdbgui::execSqltoWidget(QString sql)
 {
	 //tableInit();


	 isSave();
	 ui.tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);//设置不能编辑
	QStringList ls;
	if(dbinter.execSql(sql.toStdString())==-1)
	{
		ui.label->setText(QString::fromLocal8Bit(dbinter.sRet.data()));
		QMessageBox::warning(this,QString::fromLocal8Bit("错误"),QString::fromLocal8Bit(dbinter.sRet.data()));
		return;
	}
	dbinter.getField("",ls);
	ui.tableWidget->setColumnCount(dbinter.getColum()); //设置列数
	ui.tableWidget->setRowCount(dbinter.getRow());
	ui.tableWidget->setHorizontalHeaderLabels(ls);
	for(int i = 0; i<dbinter.getRow(); i++)
	{
		for (int j = 0;j<dbinter.getColum();j++)
		{
			QTableWidgetItem *item=new QTableWidgetItem(QString::fromLocal8Bit(dbinter.getValue(i,j).data()));
			ui.tableWidget->setItem(i,j,item);
			//V_item.push_back(QString::fromLocal8Bit(dbinter.getValue(i,j).data()));
		}
	}
	ui.label->setText(QString::fromLocal8Bit(dbinter.sRet.data()));
	//bar = ui.tableWidget->verticalScrollBar();
	//if (bar!=NULL)
	//{
	//	QObject::connect((QObject *)bar , SIGNAL(valueChanged ( int ) ) , this , SLOT(getValue(int)));
	//	int row_Hei = ui.tableWidget->rowHeight(1);                                                  //每行的高度
	//	int scrollHeight = ui.tableWidget->maximumViewportSize ().height();               //获取滚动条的滚动范围
	//	float rowN = (float)scrollHeight/(float)row_Hei;
	//	m_TabPageRowN = (int)rowN;
	//	if (rowN - m_TabPageRowN > 0.000001)
	//	{
	//		m_TabPageRowN++;
	//	}
	//}
	m_curRow = dbinter.getRow();
 }
 void sys_mdbgui::setRegistInfo()
 {
	 b_append=false;
	 if (l_reg>MAX_SHOW_SIZE)
	 {
		 ui.textEdit_2->clear();
		 l_reg = 0;
	 }

	 int i0=m_SendInfo.count();
	  sendInfo *pSi;
	 if (i0>0)
	 {
		 //if (i0>=10)
		 //{
			// i0=10;
		 //}
		 pSi=m_SendInfo[0];

	 }else
	 {
		 b_append=true;
		 return;
	 }

	 //for(int k0=0;k0<i0;k0++)
	 //{

	
	 QString s_info;
	 SString s_operate;
	 SString tableName;
	 SString s_result;
	 SPtrList<stuTableField> *sp;
	 QMap<int,unsigned int> *oidMap;
	 QTextCursor Qcur;
	 QScrollBar *v_bar;
		 
	
	 bool b_add=false;
	 bool b_update=false;
	 bool b_del=false;

	 
	 pSi=m_SendInfo[0];
	 tableName = pSi->sTable;
	 
	 switch(pSi->eType)
	 {
	 case MDB_TRG_INSERT:
		 s_operate = "插入";
		 b_add=true;
		 break;
	 case MDB_TRG_UPDATE:
		 s_operate = "更新";
		 b_update=true;
		 //refresh(pSi->sTable);
		 break;
	 case MDB_TRG_DELETE:
		 s_operate = "删除";
		 b_del=true;
		 break;
	 case MDB_TRG_TRUNCATE:
		 break;
	 case MDB_TRG_UPDATE_WITHPK:
		 break;
	 }
	 if (m_curTableName!=pSi->sTable)
	 {
		 b_update=b_del=b_add=false;
	 }
	 s_result =pSi->sDate + " "+ pSi->sTime+"::" "表"+pSi->sTable +"执行" + s_operate + "操作,";
	 s_result+=SString::toFormat("影响%d行",pSi->iTrgRows);
	 s_info = QString::fromLocal8Bit(s_result.data());

	 
	 //dbinter.getTablefields(tableName,sp);
	sp=m_vfields[m_tableMap[pSi->sTable.data()]].lf;
	 s_info +="\r\n";
	 s_info+="                     ";
	 int i_len=0;
	 int a_len=0;
	 int *max_len = new int[sp->count()];
	 memset(max_len,0,sizeof(int)*(sp->count()));
	 for(int i = 0;i<pSi->iTrgRows;i++)
	 {
		 for(int j = 0;j<sp->count();j++)
		 {
			i_len= CMdbClient::GetValueStr(pSi->pTrgData+i*pSi->iRowSize,(*sp)[j]).length();
			max_len[j]=i_len;
			if (strlen((*sp)[j]->name)>max_len[j])
			{
				max_len[j] = strlen((*sp)[j]->name);
			}
		 }           
	 }
	 for(int i = 0;i< sp->count();i++)
	 {
		 
		 s_info+=SString::toFillString((*sp)[i]->name,max_len[i],' ',false);
		 s_info+=" ";
	 }
	 s_info +="\r\n";
	 s_info+="                     ";
	 for(int i = 0;i< sp->count();i++)
	 {
		 s_info+=SString::toFillString("-",max_len[i],'-',false);
		 s_info+=" ";
	 }
	 s_info+="\r\n";
	 s_info+="                     ";
	 unsigned int uoid=0,loid=0;
	 int c_row=-1;
	 SString sfieldValue;
	 int n_row=ui.tableWidget->rowCount();
	 for(int i = 0;i<pSi->iTrgRows;i++)
	 {
		  b_edit=false;
		c_row=-1;
		 memcpy(&uoid,pSi->pTrgData+i*pSi->iRowSize,4);
		 for (int k=0;k<n_row;k++)
		 {
			 QString stext=ui.tableWidget->item(k,0)->text();
			 int isize=4>stext.size()?stext.size():4;
			 if ((*sp)[0]->type==6)
			 {
				 memcpy(&loid,stext.toStdString().data(),isize);
			 }else
			 {
				 loid=stext.toUInt();
			 }
			 if (loid==uoid)
			 {
				 c_row=k;
				 break;
			 }
		 }
		 if (b_add)
		 {
			 if (c_row!=-1)
			 {
				 b_add=false;
			 }else
			 {
				 ui.tableWidget->insertRow(n_row);				
			 }
		 }
		 if (b_del&&c_row>=0)
		 {
			 ui.tableWidget->removeRow(c_row);
		 }
		 for(int j = 0;j<sp->count();j++)
		 {
			 sfieldValue=CMdbClient::GetValueStr(pSi->pTrgData+i*pSi->iRowSize,(*sp)[j]);
			 s_info+=QString::fromLocal8Bit(SString::toFillString(sfieldValue,max_len[j],' ',false))+" ";
			 if (b_add)
			 {
				 ui.tableWidget->setItem(n_row,j,new QTableWidgetItem(sfieldValue.data()));
			 }
			 if (j!=0&&b_update)
			 {
				 if (c_row!=-1)
				 {
					 ui.tableWidget->item(c_row,j)->setText(sfieldValue.data());
				 }
			 }
		 }
		 s_info+="\r\n";
		 s_info+="                     ";
		  b_edit=true;
	 }
	 for(int i = 0;i< sp->count();i++)
	 {
		 s_info+=SString::toFillString("-",max_len[i],'-',false);
		 s_info+=" ";
	 }
	 s_info+="\r\n";
	 delete[] max_len;
	 Qcur= ui.textEdit_2->textCursor();
	 Qcur.movePosition(QTextCursor::End);
	 ui.textEdit_2->setTextCursor(Qcur);
	 ui.textEdit_2->insertPlainText(s_info);
		 m_SendInfo.remove(0);
		 l_reg+=s_info.size();
		 v_bar = ui.textEdit_2->verticalScrollBar();
		 if (v_bar!=NULL)
		 {
			 v_bar->setValue(v_bar->maximumHeight());
		 }
	 //}
	b_append=true;
 }
 void sys_mdbgui::isSave()
 {
	 if (dbinter.isCmdExec())
	 {
		 return;
	 }
	 int i_ret = QMessageBox::warning(this,QString::fromLocal8Bit("提示"),QString::fromLocal8Bit("是否保存？"),QMessageBox::Yes,QMessageBox::No);
	 if (i_ret==QMessageBox::Yes)
	 {
		on_pushButton_save_pressed();
	 }else
	 {
		 dbinter.clearRecord();
		 return;
	 }
 }
 SString sys_mdbgui::readSeting()
 {
	 SString sParam;
	 QString s_ip,s_port,s_user,s_password;
	 QSettings settings("Address.ini", QSettings::IniFormat);
	 s_ip = settings.value("USERSETTING/IP").toString();
	 s_port = settings.value("USERSETTING/PORT").toString();
	 s_user = settings.value("USERSETTING/USER").toString();
	 s_password = settings.value("USERSETTING/PASSWORD").toString();
	 sParam = SString::toFormat("hostaddr=%s;port=%d;user=%s;password=%s;",s_ip.toStdString().c_str(), s_port.toInt(),s_user.toStdString().c_str(),s_password.toStdString().c_str());
	 return sParam;
 }
 void sys_mdbgui::writeSeting(QString s_ip,QString s_port,QString s_user,QString s_password)
 {
	 QSettings settings("Address.ini", QSettings::IniFormat); // 当前目录的INI文件
	 settings.beginGroup("USERSETTING");
	 settings.setValue("IP", s_ip);
	 settings.setValue("PORT", s_port);
	 settings.setValue("USER", s_user);
	 settings.setValue("PASSWORD", s_password);
	 settings.endGroup();
 }
 void sys_mdbgui::setShortCut()
 {
	 QShortcut  *m_ALT_down_Accel= new QShortcut(QKeySequence(Qt::Key_F5), this);
	 QObject::connect(m_ALT_down_Accel,SIGNAL(activated()),this,SLOT(on_action_triggered()));
	 
 }
BYTE * registFunction(void* cbParam, SString &sTable, eMdbTriggerType eType,int iTrgRows,int iRowSize,BYTE *pTrgData)
{
	//QVariant qv;
	sendInfo *pSi = new sendInfo();
	SString sysDate = QDate::currentDate().toString("yyyy-MM-dd").toStdString();
	SString sysTime = QTime::currentTime().toString("hh:mm:ss").toStdString();
	pSi->sTable = sTable;
	pSi->eType = eType;
	pSi->iTrgRows = iTrgRows;
	pSi->iRowSize = iRowSize;
	pSi->sDate = sysDate;
	pSi->sTime = sysTime;
	pSi->pTrgData= new unsigned char[iRowSize*iTrgRows];
	memcpy(pSi->pTrgData,pTrgData,iRowSize*iTrgRows);



	//if (((sys_mdbgui*)cbParam)->b_append==true)
	//{
		if (((sys_mdbgui*)cbParam)->m_SendInfo.count()>10000)
		{
			if (((sys_mdbgui*)cbParam)->b_append==true)
			{
			((sys_mdbgui*)cbParam)->m_SendInfo.clear();
			}
		}else
		{
			((sys_mdbgui*)cbParam)->m_SendInfo.append(pSi);
		}

	//} 
	return 0;
}
