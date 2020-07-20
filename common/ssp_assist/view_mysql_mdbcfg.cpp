#include "view_mysql_mdbcfg.h"
#include "ssp_gui.h"

view_mysql_mdbcfg::view_mysql_mdbcfg(QWidget *parent)
	: CBaseView(parent)
{
	ui.setupUi(this);

	connect(ui.pushButton_ConnDB,SIGNAL(clicked()),this,SLOT(OnConnDB()));
	connect(ui.pushButton_NewCfg,SIGNAL(clicked()),this,SLOT(OnNewCfg()));
	connect(ui.pushButton_LoadCfg,SIGNAL(clicked()),this,SLOT(OnLoadCfg()));
	connect(ui.pushButton_SaveCfg,SIGNAL(clicked()),this,SLOT(OnSaveCfg()));
	connect(ui.pushButton_ExportStruct,SIGNAL(clicked()),this,SLOT(OnExportStruct()));
	connect(ui.pushButton_ExportSQL,SIGNAL(clicked()),this,SLOT(OnExportSQL()));

	connect(ui.pushButton_ImportData_SDB  ,SIGNAL(clicked()),this,SLOT(OnImportData_SDB()));
	connect(ui.pushButton_ExportData_SDB  ,SIGNAL(clicked()),this,SLOT(OnExportData_SDB()));
	connect(ui.pushButton_ExportSQL_MYSQL ,SIGNAL(clicked()),this,SLOT(OnExportSQL_MYSQL()));
	connect(ui.pushButton_ExportSQL_ORACLE,SIGNAL(clicked()),this,SLOT(OnExportSQL_ORACLE()));
	connect(ui.treeWidget,SIGNAL(itemChanged (QTreeWidgetItem*,int)),this,SLOT(slotClickTreeChkbox(QTreeWidgetItem*,int)));

	QSettings settings("SKT", "SSP_ASSIST");
	ui.lineEdit_IP->setText(settings.value("mysql_host").toString());
	ui.lineEdit_Port->setText(settings.value("mysql_port").toString());
	ui.lineEdit_DbName->setText(settings.value("mysql_dbname").toString());
	ui.lineEdit_User->setText(settings.value("mysql_user").toString());
	ui.lineEdit_Password->setText(settings.value("mysql_password").toString());
	m_DbRestore.m_pDb = &m_mysql;

}

view_mysql_mdbcfg::~view_mysql_mdbcfg()
{

}

void view_mysql_mdbcfg::OnConnDB()
{
	SString sParams;
	sParams.sprintf("hostaddr=%s;port=%s;dbname=%s;user=%s;password=%s;",
		ui.lineEdit_IP->text().toStdString().data(),
		ui.lineEdit_Port->text().toStdString().data(),
		ui.lineEdit_DbName->text().toStdString().data(),
		ui.lineEdit_User->text().toStdString().data(),
		ui.lineEdit_Password->text().toStdString().data());
	m_mysql.SetParams(sParams);
	QSettings settings("SKT", "SSP_ASSIST");
	settings.setValue("mysql_host", ui.lineEdit_IP->text());
	settings.setValue("mysql_port", ui.lineEdit_Port->text());
	settings.setValue("mysql_dbname", ui.lineEdit_DbName->text());
	settings.setValue("mysql_user", ui.lineEdit_User->text());
	settings.setValue("mysql_password", ui.lineEdit_Password->text());
	if(m_mysql.GetStatus() == SDatabase::UNCONNECT || m_mysql.GetStatus() == SDatabase::DBERROR)
	{
		//尚未连接
		if(!m_mysql.Connect())
		{
			SQt::ShowErrorBox("错误","连接MySQL数据库时失败!");
		}
		else
		{
			ui.pushButton_ConnDB->setText("断开(&D)");
			SString sFile = ui.lineEdit_CfgFile->text().toStdString().data();
			if(sFile.length() != 0)
			{
				m_XmlMdbCfg.clear();
				m_XmlMdbCfg.ReadConfig(sFile);
			}
			OnLoadTables();
		}
	}
	else
	{
		//断开连接
		m_mysql.DisConnect();
		ui.pushButton_ConnDB->setText("连接(&C)");
	}
}

void view_mysql_mdbcfg::OnNewCfg()
{
	QString fileName = QFileDialog::getSaveFileName(this,tr("MDB内存库配置文件"), "" , tr("XML文件 (*.xml)"));
	if(fileName.length() == 0)
		return;
	SString sFile = fileName.toStdString().data();
	if(sFile.right(4).toLower() != ".xml")
	{
		sFile += ".xml";
	}
	ui.lineEdit_CfgFile->setText(sFile.data());
	m_XmlMdbCfg.clear();
}

void view_mysql_mdbcfg::OnLoadCfg()
{
	QString fileName = QFileDialog::getOpenFileName(this,tr("MDB内存库配置文件"), "", tr("XML文件 (*.xml)"));
	if(fileName.length() == 0)
		return;
	SString sFile = fileName.toStdString().data();
	m_XmlMdbCfg.clear();
	if(!m_XmlMdbCfg.ReadConfig(sFile))
	{
		SQt::ShowInfoBox(tr("提示"),tr("配置文件打开失败!"));
		return;
	}
	ui.lineEdit_CfgFile->setText(sFile.data());
	OnLoadTables();
}

SString view_mysql_mdbcfg::MySqlType2Mdb(SString sType)
{
	//decimal(12,0)
	//numeric
	sType = sType.toLower();
	if(sType.left(8) == "decimal(" || sType.left(8) == "numeric(")
	{
		SString str = sType.mid(8);
		if(str.length() > 0)
			str = str.left(str.length()-1);
		int pn = SString::GetIdAttributeI(1,str,",");
		int pm = SString::GetIdAttributeI(2,str,",");
		if(pm > 0)
		{
			//有小数，统一映射为浮点数
			return "float";
		}
		else if(pn <= 3)
			return "byte";
		else if(pn <=	5)
			return "short";
		else if(pn <= 12)
			return "int";
		else
			return "int64";
	}
	else if(sType.left(4) == "int(")
	{
// 		int pn = sType.mid(4).toInt();
// 		if(pn <= 3)
// 			return "byte";
// 		else if(pn <=	5)
// 			return "short";
// 		else if(pn <= 11)
			return "int";
// 		else
// 			return "int64";
	}
	else if(sType.left(7) == "bigint(")
	{
		return "int64";
	}
	else if(sType.left(8) == "tinyint(")
	{
		return "byte";
	}
	else if(sType == "float")
	{
		return "float";
	}
	else if(sType.left(4) == "blob" || sType.left(8) == "longblob" || sType.left(9) == "mediumblob" || sType.left(9) == "shortblob" || sType == "mediumtext" || sType == "text" || sType == "longtext")
		return "blob";
	else if(sType.left(5) == "char(")
		return sType;
	else if(sType.left(8) == "varchar(")
		return sType.mid(3);
	else if(sType == "datetime" || sType == "timestamp")
	{
		return "int";
	}
	else
		return "";
}

SString view_mysql_mdbcfg::MySqlType2Oracle(SString sType)
{
	//decimal(12,0)
	//numeric
	sType = sType.toLower();
	if(sType.left(8) == "decimal(" || sType.left(8) == "numeric(")
	{
		return sType;
	}
	else if(sType.left(4) == "int(")
	{
// 		int pn = sType.mid(4).toInt();
// 		if(pn <= 3)
// 			return "number(3,0)";
// 		else if(pn <=	5)
// 			return "number(5,0)";
// 		else if(pn <= 11)
			return "number(11,0)";
// 		else
// 			return "integer";
	}
	else if(sType.left(7) == "bigint(")
	{
		return "integer";
	}
	else if(sType.left(8) == "tinyint(")
	{
		return "number(3,0)";
	}
	else if(sType == "float")
	{
		return "float";
	}
	else if(sType.left(4) == "blob" || sType.left(8) == "longblob" || sType.left(9) == "mediumblob" || sType.left(9) == "shortblob" || sType == "mediumtext" || sType == "text" || sType == "longtext")
		return "blob";
	else if(sType.left(5) == "char(")
		return "varchar2"+sType.mid(4);
	else if(sType.left(8) == "varchar(")
		return "varchar2"+sType.mid(7);
	else if(sType == "datetime" || sType == "timestamp")
	{
		return sType;
	}
	else
		return "";
}

SString view_mysql_mdbcfg::MysqlKey2Mdb(SString sKey)
{
	if(sKey == "PRI")
		return "pk";
	else if(sKey == "MUL")
		return "idx";
	else
		return "";
}

void view_mysql_mdbcfg::OnLoadTables()
{
	if(m_mysql.GetConnectHandle() == NULL)
	{
		OnConnDB();
	}
	if(m_mysql.GetConnectHandle() == NULL)
	{
		return;
	}
	SString sql;
	SRecordset rsTables;
	ui.treeWidget->clear();
	
	QStringList head;
	head<<tr("表/字段")<<tr("类型")<<tr("允许空")<<tr("键")<<tr("缺省值")<<tr("扩展属性");
	ui.treeWidget->clear();
	ui.treeWidget->setColumnCount(head.size());
	ui.treeWidget->setHeaderLabels(head);
	ui.treeWidget->setColumnWidth(0,300);
	ui.treeWidget->setColumnWidth(1,120);
	ui.treeWidget->setColumnWidth(2,60);
	ui.treeWidget->setColumnWidth(3,70);
	ui.treeWidget->setColumnWidth(4,60);
	ui.treeWidget->setColumnWidth(5,80);

	m_mysql.Retrieve("show tables",rsTables);
	int i,cnt = rsTables.GetRows();
	if(cnt == 0)
		return;
	SRecordset rsViews;
	m_mysql.Retrieve(SString::toFormat("SELECT table_name FROM information_schema.views where table_schema='%s'",
		ui.lineEdit_DbName->text().toStdString().data()),rsViews);

	SSP_GUI->SetStatusTip(SString::toFormat("共%d张数据表",cnt));
	ui.pushButton_ConnDB->setEnabled(false);
	ui.pushButton_LoadCfg->setEnabled(false);
	ui.pushButton_SaveCfg->setEnabled(false);
	ui.pushButton_NewCfg->setEnabled(false);
	ui.pushButton_ExportStruct->setEnabled(false);
	ui.pushButton_ExportSQL->setEnabled(false);
	ui.pushButton_ImportData_SDB->setEnabled(false);
	ui.pushButton_ExportData_SDB->setEnabled(false);
	ui.pushButton_ExportSQL_MYSQL->setEnabled(false);
	ui.pushButton_ExportSQL_ORACLE->setEnabled(false);
	SSP_GUI->ShowWaiting();
	QStringList sl_name;
	sl_name<<tr("所有表");
	QTreeWidgetItem* all_table = new QTreeWidgetItem(ui.treeWidget,sl_name);
	all_table->setCheckState(0,Qt::Unchecked);
	all_table->setData(0,Qt::UserRole,0);
	ui.treeWidget->addTopLevelItem(all_table);
	ui.treeWidget->expandAll();
	SBaseConfig *pTabNode,*pFldNode;
	SString tab_name;
	int j,cnt2;
	int iExpectViews = 0;
	for(i=0;i<cnt;i++)
	{
		tab_name = rsTables.GetValue(i,0).data();
		//如果是视图则忽略
		bool bView = false;
		for(j=0;j<rsViews.GetRows();j++)
		{
			if(tab_name == rsViews.GetValue(j,0))
			{
				bView = true;
				iExpectViews++;
				break;
			}
		}
		if(bView)
			continue;
		sl_name.clear();
		sl_name<<tab_name.data();
		QTreeWidgetItem* table = new QTreeWidgetItem(all_table,sl_name);
		pTabNode = m_XmlMdbCfg.SearchChild(SString::toFormat("table<name='%s'>",rsTables.GetValue(i,0).data()));
		table->setCheckState(0,pTabNode==NULL?(Qt::Unchecked):(Qt::Checked));
		/*
		field,type,null,key,default,extra
		ied_no	decimal(12,0)	NO	PRI	0	
		cpu_no	decimal(3,0)	NO	PRI	0	
		name	varchar(16)	NO			
		version	varchar(32)	YES			
		crc	varchar(16)	YES			
		version_time	varchar(16)	YES			
		mms_path	varchar(32)	YES			

		*/
		sql.sprintf("SHOW columns from %s",rsTables.GetValue(i,0).data());
		SRecordset rsFields;
		m_mysql.Retrieve(sql,rsFields);
		cnt2 = rsFields.GetRows();
		for(j=0;j<cnt2;j++)
		{
			SString sFieldName	= rsFields.GetValue(j,0);
			SString sFieldType	= rsFields.GetValue(j,1);
			SString sIsNull		= rsFields.GetValue(j,2);
			SString sKey		= rsFields.GetValue(j,3);
			SString sDefault	= rsFields.GetValue(j,4);
			SString sExtra		= rsFields.GetValue(j,5);
			sl_name.clear();
			sl_name<<sFieldName.data()<<sFieldType.data()<<sIsNull.data()<<sKey.data()<<sDefault.data()<<sExtra.data();
			QTreeWidgetItem* field = new QTreeWidgetItem(table,sl_name);
			if(MySqlType2Mdb(sFieldType).length() == 0)
			{
				table->setBackground(1,QBrush(QColor(255,0,0)));
				field->setForeground(1,QBrush(QColor(255,0,0)));
			}
			if(pTabNode == NULL)
				pFldNode = NULL;
			else
				pFldNode = pTabNode->SearchChild(SString::toFormat("field<name='%s'>",sFieldName.data()));
			field->setCheckState(0,pFldNode==NULL?(Qt::Unchecked):(Qt::Checked));
		}
		ui.treeWidget->collapseItem(table);
		SSP_GUI->SetStatusTip(SString::toFormat("已加载[%d/%d]张数据表...",i+1,cnt));
		SQt::ProcessUiEvents();
	}
	SSP_GUI->HideWaiting();
	SSP_GUI->SetStatusTip(SString::toFormat("共%d张数据表信息加载完毕%s!",cnt-iExpectViews,iExpectViews==0?"": (SString::toFormat("(%d张视图已排除)",iExpectViews).data()) ));
	ui.pushButton_ConnDB->setEnabled(true);
	ui.pushButton_LoadCfg->setEnabled(true);
	ui.pushButton_SaveCfg->setEnabled(true);
	ui.pushButton_NewCfg->setEnabled(true);
	ui.pushButton_ExportStruct->setEnabled(true);
	ui.pushButton_ExportSQL->setEnabled(true);
	ui.pushButton_ImportData_SDB->setEnabled(true);
	ui.pushButton_ExportData_SDB->setEnabled(true);
	ui.pushButton_ExportSQL_MYSQL->setEnabled(true);
	ui.pushButton_ExportSQL_ORACLE->setEnabled(true);
}


void view_mysql_mdbcfg::slotClickTreeChkbox(QTreeWidgetItem* item ,int col)
{
	Qt::CheckState flag = item->checkState(col);//Qt::Unchecked;
	if( item->childCount()  == 0)
	{
		return;
	}
	for( int i=0;i<item->childCount();i++)
	{
		QTreeWidgetItem* ptmp = item->child(i);
		ptmp->setCheckState(0,flag);
	}
}

void view_mysql_mdbcfg::OnSaveCfg()
{
	QTreeWidgetItem *pRoot = ui.treeWidget->topLevelItem(0);
	if(pRoot == NULL)
		return;
	SString sFile = ui.lineEdit_CfgFile->text().toStdString().data();
	if(sFile.length() == 0)
	{
		SQt::ShowErrorBox(tr("错误"),tr("尚未新建或加载MDB内存库配置文件!无法进行保存!"));
		return;
	}
	m_XmlMdbCfg.clear();
	m_XmlMdbCfg.SetNodeName("mdb-define");
	int iTable,iTables = pRoot->childCount();
	for(iTable=0;iTable<iTables;iTable++)
	{
		QTreeWidgetItem *pTable = pRoot->child(iTable);
		if(pTable == NULL || pTable->checkState(0) == Qt::Unchecked)
			continue;
		SBaseConfig *pTabNode = m_XmlMdbCfg.AddChildNode("table");
		pTabNode->SetAttribute("name",pTable->text(0).toStdString().data());
		pTabNode->SetAttribute("sync_record","true");
		int iField,iFields = pTable->childCount();
		for(iField=0;iField<iFields;iField++)
		{
			QTreeWidgetItem *pField = pTable->child(iField);
			if(pField == NULL || pField->checkState(0) == Qt::Unchecked)
				continue;
			SBaseConfig *pFldNode = pTabNode->AddChildNode("field");
			pFldNode->SetAttribute("name",pField->text(0).toStdString().data());
			pFldNode->SetAttribute("type",MySqlType2Mdb(pField->text(1).toStdString().data()));
			pFldNode->SetAttribute("isnull",pField->text(2).toStdString().data());
			pFldNode->SetAttribute("key",MysqlKey2Mdb(pField->text(3).toStdString().data()));
			pFldNode->SetAttribute("default",pField->text(4).toStdString().data());
		}
	}
	if(!m_XmlMdbCfg.SaveConfig(sFile))
	{
		SQt::ShowErrorBox(tr("错误"),tr("保存文件时失败!"));
		return;
	}
	else
	{
		SQt::ShowInfoBox(tr("提示"),tr("文件保存成功!"));
	}
}

void view_mysql_mdbcfg::OnExportStruct()
{
	QTreeWidgetItem *pRoot = ui.treeWidget->topLevelItem(0);
	if(pRoot == NULL)
		return;
	QString fileName = QFileDialog::getSaveFileName(this,tr("MDB内存库结构定义文件"), "" , tr("C Header files (*.h)"));
	if(fileName.length() == 0)
		return;
	SString sFile = fileName.toStdString().data();
	if(sFile.right(2).toLower() != ".h")
		sFile += ".h";
	SFile file(sFile);
	if(!file.open(IO_Truncate))
	{
		SQt::ShowErrorBox(tr("错误"),tr("保存文件时失败!"));
		return;
	}
	SString sShortName = sFile;
	if(sShortName.findRev("\\") > 0)
		sShortName = sShortName.mid(sShortName.findRev("\\")+1);
	if(sShortName.findRev("/") > 0)
		sShortName = sShortName.mid(sShortName.findRev("/")+1);
	
	SDateTime now = SDateTime::currentDateTime();
	file.fprintf("/**\r\n"
		" *\r\n"
		" * 文 件 名 : %s\r\n"
		" * 创建日期 : %s\r\n"
		" * 作    者 : SSP_ASSIST(skt001@163.com)\r\n"
		" * 修改日期 : $Date: $\r\n"
		" * 当前版本 : $Revision: $\r\n"
		" * 功能描述 : \r\n"
		" * 修改记录 : \r\n"
		" *            $Log: $\r\n"
		" *\r\n"
		" * Ver  Date        Author       Comments\r\n"
		" * ---  ----------  -----------  -------------------------------------------\r\n"
		" * 001	%s  SSP_ASSIST  　由SSP_ASSIST自动生成文件\r\n"
		" *\r\n"
		" **/\r\n\r\n",
		sShortName.data(),
		now.toString("yyyy-MM-dd hh:mm").data(),
		now.toString("yyyy-MM-dd").data()
		);
	SString sMacro = sShortName;
	sMacro.replace(".","_");
	sMacro = sMacro.toUpper();
	file.fprintf("#ifndef __MDB_STRUCT_DEF_%s__\r\n"
		"#define __MDB_STRUCT_DEF_%s__\r\n\r\n"
		"#pragma pack(push, Old, 1)\r\n"
		"\r\n",
		sMacro.data(),sMacro.data());
	int iTable,iTables = pRoot->childCount();
	for(iTable=0;iTable<iTables;iTable++)
	{
		QTreeWidgetItem *pTable = pRoot->child(iTable);
		if(pTable == NULL || pTable->checkState(0) == Qt::Unchecked)
			continue;
		file.fprintf("struct %s\r\n"
			"{\r\n",
			pTable->text(0).toStdString().data());
		int iField,iFields = pTable->childCount();
		for(iField=0;iField<iFields;iField++)
		{
			QTreeWidgetItem *pField = pTable->child(iField);
			if(pField == NULL || pField->checkState(0) == Qt::Unchecked)
				continue;
			SString name = pField->text(0).toStdString().data();
			SString sType = MySqlType2Mdb(pField->text(1).toStdString().data());

			SString sLine;
			if(sType.left(5) == "char(")
			{
				sLine.sprintf("char          %s[%d]",name.data(),sType.mid(5).toInt());
			}
			else
			{
				if(sType == "byte")
					sLine = "unsigned char ";
				else if(sType == "short")
					sLine = "short         ";
				else if(sType == "int")
					sLine = "int           ";
				else if(sType == "int64")
					sLine = "INT64         ";
				else if(sType == "float")
					sLine = "float         ";
				else if(sType == "blob")
					sLine = "unsigned int  ";
				else 
					sLine = "//unknown     ";
				sLine += name;
			}
			file.fprintf("\t%s;\r\n",sLine.data());
		}
		file.fprintf("};\r\n\r\n");
	}
	file.fprintf("#pragma pack(pop, Old)\r\n\r\n"
				"#endif//__MDB_STRUCT_DEF_%s__\r\n",
		sMacro.data(),sMacro.data());
	file.close();
	SQt::ShowInfoBox(tr("提示"),tr("文件保存成功!"));
}


void view_mysql_mdbcfg::OnExportSQL()
{
	QTreeWidgetItem *pRoot = ui.treeWidget->topLevelItem(0);
	if(pRoot == NULL)
		return;
	QString fileName = QFileDialog::getSaveFileName(this,tr("MDB内存库建表SQL文件"), "" , tr("SQL files (*.sql)"));
	if(fileName.length() == 0)
		return;
	SString sFile = fileName.toStdString().data();
	if(sFile.right(4).toLower() != ".sql")
		sFile += ".sql";
	SFile file(sFile);
	if(!file.open(IO_Truncate))
	{
		SQt::ShowErrorBox(tr("错误"),tr("保存文件时失败!"));
		return;
	}
	SString sShortName = sFile;
	if(sShortName.findRev("\\") > 0)
		sShortName = sShortName.mid(sShortName.findRev("\\")+1);
	if(sShortName.findRev("/") > 0)
		sShortName = sShortName.mid(sShortName.findRev("/")+1);
	
	SDateTime now = SDateTime::currentDateTime();
	file.fprintf("/*########################################################################\r\n"
				"#*\r\n"
				"#* 文 件 名 : %s\r\n"
				"#* 创建日期 : %s\r\n"
				"#* 作    者 : SSP_ASSIST(skt001@163.com)\r\n"
				"#* 修改日期 : $Date:$\r\n"
				"#* 当前版本 : $Revision: 001$\r\n"
				"#* 功能描述 : MDB语法的数据库结构SQL脚本\r\n"
				"#* 修改记录 : \r\n"
				"#*            $Log: $\r\n"
				"#*\r\n"
				"#* Ver  Date        Author      Comments\r\n"
				"#* ---  ----------  ----------  -------------------------------------------\r\n"
				"#* 001	%s  SSP_ASSIST  由SSP01平台的SSP_ASSIS自动生成\r\n"
				"#*\r\n"
				"########################################################################*/\r\n"
				"\r\n\r\n",
		sShortName.data(),
		now.toString("yyyy-MM-dd hh:mm").data(),
		now.toString("yyyy-MM-dd").data()
		);
	
	int iTable,iTables = pRoot->childCount();
	SString sLine;
	for(iTable=0;iTable<iTables;iTable++)
	{
		QTreeWidgetItem *pTable = pRoot->child(iTable);
		if(pTable == NULL || pTable->checkState(0) == Qt::Unchecked)
			continue;
		SString tablename = pTable->text(0).toStdString().data();
		sLine.sprintf("drop table %s;\r\n"
			"create table %s\r\n"
			"(\r\n",
			tablename.data(),tablename.data());
		int iField,iFields = pTable->childCount();
		SString sKeyName;
		for(iField=0;iField<iFields;iField++)
		{
			QTreeWidgetItem *pField = pTable->child(iField);
			if(pField == NULL || pField->checkState(0) == Qt::Unchecked)
				continue;
			SString name = pField->text(0).toStdString().data();
			SString sType = MySqlType2Mdb(pField->text(1).toStdString().data());
			SString sIsNull = pField->text(2).toStdString().data();
			SString sKey = pField->text(3).toStdString().data();
			sLine += SString::toFormat("  %s %s%s,\r\n",name.data(),sType.data(),sIsNull=="NO"?" not null":"");
			if(sKey == "PRI")
			{
				if(sKeyName.length() > 0)
					sKeyName += ",";
				sKeyName += name;
			}
		}

		//如果没有主健就算了，不自动生成主键，防止生成的主键不满足要求
		if(sKeyName.length() == 0)
		{
			//将最后一个字段的逗号改为空格
			(sLine.data())[sLine.length()-3] = ' ';
		}
		else
		{
	// 		//处理主键,如果没有明确指定主键，则将所有字段一起作为主健
	// 		if(sKeyName.length() == 0)
	// 		{
	// 			for(iField=0;iField<iFields;iField++)
	// 			{
	// 				QTreeWidgetItem *pField = pTable->child(iField);
	// 				if(pField == NULL || pField->checkState(0) == Qt::Unchecked)
	// 					continue;
	// 				SString name = pField->text(0).toStdString().data();
	// 				if(sKeyName.length() > 0)
	// 					sKeyName += ",";
	// 				sKeyName += name;
	// 			}
	// 		}
			sLine += SString::toFormat("  constraint pk_%s primary key(%s)\r\n",tablename.data(),sKeyName.data());
		}
		sLine += SString::toFormat(");\r\n\r\n");
		file.writeBlock((BYTE*)sLine.data(),sLine.length());
		sLine = "";
	}
	file.close();
	SQt::ShowInfoBox(tr("提示"),tr("文件保存成功!"));
}

void view_mysql_mdbcfg::OnExportSQL_MYSQL()
{
	QTreeWidgetItem *pRoot = ui.treeWidget->topLevelItem(0);
	if(pRoot == NULL)
		return;
	QString fileName = QFileDialog::getSaveFileName(this,tr("MySQL数据库建表SQL文件"), "" , tr("SQL files (*.sql)"));
	if(fileName.length() == 0)
		return;
	SString sFile = fileName.toStdString().data();
	if(sFile.right(4).toLower() != ".sql")
		sFile += ".sql";
	SFile file(sFile);
	if(!file.open(IO_Truncate))
	{
		SQt::ShowErrorBox(tr("错误"),tr("保存文件时失败!"));
		return;
	}
	SString sShortName = sFile;
	if(sShortName.findRev("\\") > 0)
		sShortName = sShortName.mid(sShortName.findRev("\\")+1);
	if(sShortName.findRev("/") > 0)
		sShortName = sShortName.mid(sShortName.findRev("/")+1);
	
	SDateTime now = SDateTime::currentDateTime();
	file.fprintf("-- ########################################################################\r\n"
				"-- #*\r\n"
				"-- #* 文 件 名 : %s\r\n"
				"-- #* 创建日期 : %s\r\n"
				"-- #* 作    者 : SSP_ASSIST(skt001@163.com)\r\n"
				"-- #* 修改日期 : $Date:$\r\n"
				"-- #* 当前版本 : $Revision: 001$\r\n"
				"-- #* 功能描述 : MySQL语法的数据库结构SQL脚本\r\n"
				"-- #* 修改记录 : \r\n"
				"-- #*            $Log: $\r\n"
				"-- #*\r\n"
				"-- #* Ver  Date        Author      Comments\r\n"
				"-- #* ---  ----------  ----------  -------------------------------------------\r\n"
				"-- #* 001  %s  SSP_ASSIST  由SSP01平台的SSP_ASSIS自动生成\r\n"
				"-- #*\r\n"
				"-- ########################################################################\r\n"
				"\r\n\r\n"
				"/*!40101 SET NAMES gb2312 */;\r\n\r\n",
		sShortName.data(),
		now.toString("yyyy-MM-dd hh:mm").data(),
		now.toString("yyyy-MM-dd").data()
		);
	
	int iTable,iTables = pRoot->childCount();
	SString sLine;
	SString sql;
	SRecordset rs;
	for(iTable=0;iTable<iTables;iTable++)
	{
		QTreeWidgetItem *pTable = pRoot->child(iTable);
		if(pTable == NULL || pTable->checkState(0) == Qt::Unchecked)
			continue;
		sLine = "";
		SString tablename = pTable->text(0).toStdString().data();
		sLine += SString::toFormat(
			"-- ########################################################################\r\n"
			"-- Table structure for %s\r\n"
			"-- ########################################################################\r\n"
			"drop table if exists `%s`;\r\n"
			"create table `%s`\r\n"
			"(\r\n",
			tablename.data(),tablename.data(),tablename.data());
		int iField,iFields = pTable->childCount();
		SString sKeyName;
		for(iField=0;iField<iFields;iField++)
		{
			QTreeWidgetItem *pField = pTable->child(iField);
// 			if(pField == NULL || pField->checkState(0) == Qt::Unchecked)
// 				continue;
			SString name = pField->text(0).toStdString().data();
			SString sType = pField->text(1).toStdString().data();
			SString sIsNull = pField->text(2).toStdString().data();
			SString sKey = pField->text(3).toStdString().data();
			SString sDefault = pField->text(4).toStdString().data();
			SString sExtra = pField->text(5).toStdString().data();
			if(sDefault.length() > 0)
				sDefault = SString::toFormat(" default '"+sDefault+"'");
			else if(sIsNull!="NO")
				sDefault =  " default null";
			if(sExtra.length() > 0)
				sExtra = " "+sExtra;
			sLine += SString::toFormat(
				"  `%s` %s%s%s%s,\r\n",
				name.data(),sType.data(),sIsNull=="NO"?" not null":"",
				sDefault.data(),sExtra.data());
			if(sKey == "PRI")
			{
				if(sKeyName.length() > 0)
					sKeyName += ",";
				sKeyName += "`";
				sKeyName += name;
				sKeyName += "`";
			}
		}
		//如果没有主健就算了，不自动生成主键，防止生成的主键不满足要求
		bool bHavePk = true;
		if(sKeyName.length() == 0)
		{
			bHavePk = false;
			//将最后一个字段的逗号改为空格
			(sLine.data())[sLine.length()-3] = ' ';
		}
		else
		{
			//处理主键,如果没有明确指定主键，则将所有字段一起作为主健
// 			if(sKeyName.length() == 0)
// 			{
// 				bHavePk = false;
// 				for(iField=0;iField<iFields;iField++)
// 				{
// 					QTreeWidgetItem *pField = pTable->child(iField);
// 					if(pField == NULL || pField->checkState(0) == Qt::Unchecked)
// 						continue;
// 					SString name = pField->text(0).toStdString().data();
// 					if(sKeyName.length() > 0)
// 						sKeyName += ",";
// 					sKeyName += name;
// 				}
// 			}
			sLine += SString::toFormat("  primary key(%s)",sKeyName.data());
		}



		//创建索引
		sql.sprintf("show index from `%s` where key_name<>'PRIMARY'",tablename.data());
		m_mysql.Retrieve(sql,rs);
		SString sLastKeyName="";
		int icols = 0;
		int fromidx = bHavePk?1:0;
		for(int i=fromidx;i<rs.GetRows();i++)
		{
			int non_unique = rs.GetValue(i,1).toInt();//0:unique 1:normal
			SString key_name = rs.GetValue(i,2);
			SString col_name = rs.GetValue(i,4);
			if(key_name != sLastKeyName)
			{
				sLastKeyName = key_name;
				if(i > fromidx)
					sLine += ")";
				icols = 0;
				sLine += SString::toFormat(",\r\n  %skey %s (",(non_unique==0?"unique ":""),key_name.data());
			}
			if(icols > 0)
				sLine += ",";
			icols++;
			sKeyName += "`";
			sLine += col_name;
			sKeyName += "`";
		}
		if(rs.GetRows() > fromidx)
			sLine += ")\r\n";
		else
			sLine += "\r\n";
		sLine += SString::toFormat(") engine=innodb default charset=utf8;\r\n\r\n");
		file.writeBlock((BYTE*)sLine.data(),sLine.length());
	}
	file.close();
	SQt::ShowInfoBox(tr("提示"),tr("文件保存成功!"));
}

void view_mysql_mdbcfg::OnExportSQL_ORACLE()
{
	QTreeWidgetItem *pRoot = ui.treeWidget->topLevelItem(0);
	if(pRoot == NULL)
		return;
	QString fileName = QFileDialog::getSaveFileName(this,tr("Oracle数据库建表SQL文件"), "" , tr("SQL files (*.sql)"));
	if(fileName.length() == 0)
		return;
	SString sFile = fileName.toStdString().data();
	if(sFile.right(4).toLower() != ".sql")
		sFile += ".sql";
	SFile file(sFile);
	if(!file.open(IO_Truncate))
	{
		SQt::ShowErrorBox(tr("错误"),tr("保存文件时失败!"));
		return;
	}
	SString sShortName = sFile;
	if(sShortName.findRev("\\") > 0)
		sShortName = sShortName.mid(sShortName.findRev("\\")+1);
	if(sShortName.findRev("/") > 0)
		sShortName = sShortName.mid(sShortName.findRev("/")+1);

	SDateTime now = SDateTime::currentDateTime();
	file.fprintf("-- ########################################################################\r\n"
		"-- #*\r\n"
		"-- #* 文 件 名 : %s\r\n"
		"-- #* 创建日期 : %s\r\n"
		"-- #* 作    者 : SSP_ASSIST(skt001@163.com)\r\n"
		"-- #* 修改日期 : $Date:$\r\n"
		"-- #* 当前版本 : $Revision: 001$\r\n"
		"-- #* 功能描述 : Oracle语法的数据库结构SQL脚本\r\n"
		"-- #* 修改记录 : \r\n"
		"-- #*            $Log: $\r\n"
		"-- #*\r\n"
		"-- #* Ver  Date        Author      Comments\r\n"
		"-- #* ---  ----------  ----------  -------------------------------------------\r\n"
		"-- #* 001  %s  SSP_ASSIST  由SSP01平台的SSP_ASSIS自动生成\r\n"
		"-- #*\r\n"
		"-- ########################################################################\r\n"
		"\r\n\r\n",
		sShortName.data(),
		now.toString("yyyy-MM-dd hh:mm").data(),
		now.toString("yyyy-MM-dd").data()
		);

	int iTable,iTables = pRoot->childCount();
	SString sLine;
	SString sql;
	SRecordset rs;
	for(iTable=0;iTable<iTables;iTable++)
	{
		QTreeWidgetItem *pTable = pRoot->child(iTable);
		if(pTable == NULL || pTable->checkState(0) == Qt::Unchecked)
			continue;
		sLine = "";
		SString tablename = pTable->text(0).toStdString().data();
		sLine += SString::toFormat(
			"-- ########################################################################\r\n"
			"-- Table structure for %s\r\n"
			"-- ########################################################################\r\n"
			"prompt Creating table %s ...\r\n"
			"drop table %s;\r\n"
			"create table %s\r\n"
			"(\r\n",
			tablename.data(),tablename.data(),tablename.data(),tablename.data());
		int iField,iFields = pTable->childCount();
		SString sKeyName;
		for(iField=0;iField<iFields;iField++)
		{
			QTreeWidgetItem *pField = pTable->child(iField);
			// 			if(pField == NULL || pField->checkState(0) == Qt::Unchecked)
			// 				continue;
			SString name = pField->text(0).toStdString().data();
			SString sType = MySqlType2Oracle(pField->text(1).toStdString().data());
			SString sIsNull = pField->text(2).toStdString().data();
			SString sKey = pField->text(3).toStdString().data();
			SString sDefault = pField->text(4).toStdString().data();
			SString sExtra = pField->text(5).toStdString().data();
			if(sDefault.length() > 0)
				sDefault = SString::toFormat(" default '"+sDefault+"'");
			//else if(sIsNull!="NO")
			//	sDefault =  " default null";
			//if(sExtra.length() > 0)
			//	sExtra = " "+sExtra;
			sExtra = "";
			if(iField > 0)
				sLine += ",\r\n";
			sLine += SString::toFormat(
				"  %s %s%s%s%s",
				name.data(),sType.data(),
				sDefault.data(),
				sIsNull=="NO"?" not null":"",
				sExtra.data());
			if(sKey == "PRI")
			{
				if(sKeyName.length() > 0)
					sKeyName += ",";
				sKeyName += name;
			}
		}

		//处理主键,如果没有明确指定主键，则将所有字段一起作为主健
		bool bHavePk = true;
		if(sKeyName.length() == 0)
		{
			bHavePk = false;
// 			for(iField=0;iField<iFields;iField++)
// 			{
// 				QTreeWidgetItem *pField = pTable->child(iField);
// 				if(pField == NULL || pField->checkState(0) == Qt::Unchecked)
// 					continue;
// 				SString name = pField->text(0).toStdString().data();
// 				if(sKeyName.length() > 0)
// 					sKeyName += ",";
// 				sKeyName += name;
// 			}
		}
		sLine += "\r\n);\r\n";
		if(bHavePk)
		{
			sLine += SString::toFormat("alter table %s add constraint pk_%s primary key (%s);\r\n",
				tablename.data(),tablename.data(),sKeyName.data());
		}
		
		//创建索引
		sql.sprintf("show index from %s where key_name<>'PRIMARY'",tablename.data());
		m_mysql.Retrieve(sql,rs);
		SString sLastKeyName="";
		int icols = 0;
		int fromidx = bHavePk?1:0;
		for(int i=fromidx;i<rs.GetRows();i++)
		{
			int non_unique = rs.GetValue(i,1).toInt();//0:unique 1:normal
			SString key_name = rs.GetValue(i,2);
			SString col_name = rs.GetValue(i,4);
			if(key_name != sLastKeyName)
			{
				sLastKeyName = key_name;
				if(i > fromidx)
					sLine += ");\r\n";
				icols = 0;
				sLine += SString::toFormat("create %sindex %s on %s (",(non_unique==0?"unique ":""),key_name.data(),tablename.data());
			}
			if(icols > 0)
				sLine += ",";
			icols++;
			sLine += col_name;
		}
		if(rs.GetRows() > fromidx)
			sLine += ");\r\n";
		sLine += "\r\n";
		file.writeBlock((BYTE*)sLine.data(),sLine.length());
	}
	file.close();
	SQt::ShowInfoBox(tr("提示"),tr("文件保存成功!"));
}

void view_mysql_mdbcfg::OnExportData_SDB()
{
	//导出数据到SDB文件
	QTreeWidgetItem *pRoot = ui.treeWidget->topLevelItem(0);
	if(pRoot == NULL)
		return;
	QString fileName = QFileDialog::getSaveFileName(this,tr("待导出的SSP平台SDB数据库备份文件"), "" , tr("SDB files (*.sdb)"));
	if(fileName.length() == 0)
		return;
	SString sFile = fileName.toStdString().data();
	if(sFile.right(4).toLower() != ".sdb")
		sFile += ".sdb";
	SFile file(sFile);
	if(!file.open(IO_Truncate))
	{
		SQt::ShowErrorBox(tr("错误"),tr("保存文件时失败!"));
		return;
	}
	SString sShortName = sFile;
	if(sShortName.findRev("\\") > 0)
		sShortName = sShortName.mid(sShortName.findRev("\\")+1);
	if(sShortName.findRev("/") > 0)
		sShortName = sShortName.mid(sShortName.findRev("/")+1);
	SSP_GUI->ShowWaiting();
	SDateTime now = SDateTime::currentDateTime();
	file.fprintf("<?xml version=\"1.0\" encoding=\"GBK\"?>\r\n"
		"<!--\r\n"
		"########################################################################\r\n"
		"#*\r\n"
		"#* 文 件 名 : %s\r\n"
		"#* 创建日期 : %s\r\n"
		"#* 作    者 : SSP_ASSIST(skt001@163.com)\r\n"
		"#* 修改日期 : $Date:$\r\n"
		"#* 当前版本 : $Revision: 001$\r\n"
		"#* 功能描述 : SSP平台数据库备份文件(SDB)\r\n"
		"#* 修改记录 : \r\n"
		"#*            $Log: $\r\n"
		"#*\r\n"
		"#* Ver  Date        Author      Comments\r\n"
		"#* ---  ----------  ----------  -------------------------------------------\r\n"
		"#* 001  %s  SSP_ASSIST  由SSP01平台的SSP_ASSIS自动生成\r\n"
		"#*\r\n"
		"########################################################################\r\n"
		"-->\r\n\r\n"
		"<ssp_db>\r\n",
		sShortName.data(),
		now.toString("yyyy-MM-dd hh:mm").data(),
		now.toString("yyyy-MM-dd").data()
		);

	int iTable,iTables = pRoot->childCount();
	SString sLine;
	SString sql;
	SRecordset rs;
	int iSaveTabs=0;
	int iSaveRows=0;
	for(iTable=0;iTable<iTables;iTable++)
	{
		QTreeWidgetItem *pTable = pRoot->child(iTable);
		if(pTable == NULL || pTable->checkState(0) == Qt::Unchecked)
			continue;
		sLine = "";
		SString tablename = pTable->text(0).toStdString().data();

		sLine += SString::toFormat("  <table name=\"%s\">\r\n",tablename.data());
		SString select_field;
		SStringList blob_fields;
		SStringList pk_fields;
		SStringList type_fields;
		SValueList<int> pk_field_idx;
		SValueList<bool> isnumber_field;
		int iField,iFields = pTable->childCount();
		for(iField=0;iField<iFields;iField++)
		{
			QTreeWidgetItem *pField = pTable->child(iField);
			SString name = pField->text(0).toStdString().data();
			SString sType = pField->text(1).toStdString().data();
			SString sIsNull = pField->text(2).toStdString().data();
			SString sKey = pField->text(3).toStdString().data();
			SString sDefault = pField->text(4).toStdString().data();
			SString sExtra = pField->text(5).toStdString().data();
			type_fields.append(sType);
			bool isnumber = (sType.find("decimal")>=0 || sType.find("number")>=0 || sType.find("int")>=0 || sType.find("float")>=0);
			isnumber_field.append(isnumber);
			sLine += SString::toFormat("    <column name=\"%s\" type=\"%s\" isnull=\"%s\" key=\"%s\" default=\"%s\" extattr=\"%s\" />\r\n",
				name.data(),sType.data(),sIsNull.data(),sKey.data(),sDefault.data(),sExtra.data());
			if(select_field.length() > 0)
				select_field += ",";
			if(MySqlType2Oracle(sType).toLower().find("blob") < 0)
				select_field += name;
			else
			{
				select_field += "null";
				blob_fields.append(name);
			}
			if(sKey == "PRI")
			{
				pk_fields.append(name);
				pk_field_idx.append(iField);
			}
		}

		//如果没有定义主键，则将所有字段作为主健使用,BLOB字段除外
		if(pk_fields.count() == 0)
		{
			for(iField=0;iField<iFields;iField++)
			{
				QTreeWidgetItem *pField = pTable->child(iField);
				SString name = pField->text(0).toStdString().data();
				SString sType = pField->text(1).toStdString().data();
				if(sType.toLower().find("blob") < 0)
				{
					pk_fields.append(name);
					pk_field_idx.append(iField);
				}
			}
		}

		//write rows
		{
			bool bSelectError = false;
			MYSQL *pMySQL = m_mysql.GetConnectHandle();
			SRecordset rs;
			int offset=0;
			int len = 10000;
			int iRowSize = 0;
			unsigned long pos;
			while(1)
			{
				sql.sprintf("select %s from %s limit %d,%d",select_field.data(),tablename.data(),offset,len);
				int ret = mysql_query(pMySQL,sql.data());
				if(ret != 0)
				{
					LOGERROR("Retrieve mysql_query error:%s",mysql_error(pMySQL));
					bSelectError = true;
					break;
				}

				MYSQL_RES *pRes;
				if (!(pRes=mysql_store_result(pMySQL)))
				{
					LOGERROR("Couldn't get result from %s",mysql_error(pMySQL));
					bSelectError = true;
					break;
				}

				int cols = (int)mysql_num_fields(pRes);
				int rows = (int)mysql_num_rows(pRes);
				int i,j;
				MYSQL_ROW row;
				stuTableField *pFld;
				row= mysql_fetch_row(pRes);
				for(i=0;row != NULL && i<rows;i++)
				{
					sLine += "\t<r><![CDATA[";
					for(j=0;j<cols;j++)
					{
						if(j > 0)
							sLine += ",";
						if(row[j] == NULL)
						{
							if(type_fields[j]->find("char")>=0)
								sLine += "''";
							else
								sLine += "null";
						}
						else
						{
							if(*isnumber_field[j])
							{
								sLine += row[j];
							}
							else
							{
// 								if(type_fields[j]->find("date")=0)
// 								{
// 									sLine += "to_date(";
// 									sLine = row[j];
// 									sLine += ",'yyyy-mm-dd hh24:mi:ss')";
// 								}
// 								else if(type_fields[j]->find("timestamp")>=0)
// 								{
// 									sLine += "to_timestamp(";
// 									sLine = row[j];
// 									sLine += ",'yyyy-mm-dd hh24:mi:ss')";
// 								}
// 								else
								{
									sLine += "'";
									if(strstr(row[j],"'") != NULL)
									{
										sql = row[j];
										sql.replace("'","''");
										sLine += sql;
									}
									else
										sLine += row[j];
									sLine += "'";
								}
							}
						}					
					}
					sLine += "]]>";
					if(blob_fields.count() == 0)
						sLine += "</r>\r\n";
					else
					{
						//有blob字段
						sLine += "\r\n";
						for(j=0;j<blob_fields.count();j++)
						{
							BYTE *pBlobBuffer = NULL;
							int iBlobBufferSize = 0;
							//按主键字段查找
							sql = "";
							for(int k=0;k<pk_fields.count();k++)
							{
								if(sql.length() > 0)
									sql += " and ";
								sql += *pk_fields.at(k);
								sql += "='";
								sql += row[*pk_field_idx.at(k)];
								sql += "'";
							}
							if(m_mysql.ReadLobToMem(tablename,*blob_fields.at(j),sql,pBlobBuffer,iBlobBufferSize) && iBlobBufferSize > 0)
							{
								sLine += SString::toFormat("      <blob name=\"%s\" size=\"%d\" where=\"%s\"><![CDATA[",
									blob_fields.at(j)->data(),iBlobBufferSize,sql.data());
								if(pBlobBuffer != NULL && iBlobBufferSize > 0)
									sLine += SString::HexToStr(pBlobBuffer,iBlobBufferSize,false);
								sLine += "]]></blob>\r\n";
							}

						}
						sLine += "    </r>\r\n";
					}
					iSaveRows ++;
					if(sLine.length() > 65536)
					{
						file.writeBlock((BYTE*)sLine.data(),sLine.length());
						sLine = "";
						//SSP_GUI->SetStatusTip(SString::toFormat("已导出%d张表,共%d条记录...",iSaveTabs,iSaveRows));
						SSP_GUI->SetStatusTip(SString::toFormat("导出%d张表,共%d条记录...  当前表%s,导出%d/%d条记录...",iSaveTabs,iSaveRows,tablename.data(),i,rows));
						SQt::ProcessUiEvents();
					}
					row= mysql_fetch_row(pRes);
				}
				
				mysql_free_result(pRes);
				
				if(rows < len)
					break;
				offset += len;
			}
		}
		sLine += "  </table>\r\n";
		file.writeBlock((BYTE*)sLine.data(),sLine.length());
		sLine = "";
		iSaveTabs++;
		SSP_GUI->SetStatusTip(SString::toFormat("已导出%d张表,共%d条记录...",iSaveTabs,iSaveRows));
		SQt::ProcessUiEvents();
	}
	file.fprintf("</ssp_db>\r\n");
	SSP_GUI->HideWaiting();
	file.close();
	SSP_GUI->SetStatusTip(SString::toFormat("累计导出%d张表,共%d条记录!",iSaveTabs,iSaveRows));
	SQt::ShowInfoBox(tr("提示"),SString::toFormat("记录导出成功!累计导出%d张表,共%d条记录!",iSaveTabs,iSaveRows).data());
}

void view_mysql_mdbcfg::OnImportData_SDB()
{
	QString fileName = QFileDialog::getOpenFileName(this,tr("待导入的SSP平台SDB数据库备份文件"), "", tr("SDB文件 (*.sdb)"));
	if(fileName.length() == 0)
		return;
	m_DbRestore.m_bTrunkTab = (ui.checkBox_TrunkTabAtRest->checkState() == Qt::Checked);
	SString sFile = fileName.toStdString().data();
	if(!m_DbRestore.ParseXmlFile(sFile.data()))
	{
		SQt::ShowErrorBox(tr("错误"),tr("导入数据库备份时失败!"));
		return;
	}
	SSP_GUI->ShowWaiting();
	if(m_DbRestore.m_sql.length() > 0)
		m_DbRestore.ProcessEndSql();
	SSP_GUI->HideWaiting();
	SSP_GUI->SetStatusTip(SString::toFormat("累计导入%d张表,共%d条记录!%s",
		m_DbRestore.m_iSaveTabs,m_DbRestore.m_iSaveRows,m_DbRestore.m_iDbErrors==0?"":(SString::toFormat("其中%d批SQL指令失败!",m_DbRestore.m_iDbErrors).data())));
	SQt::ShowInfoBox(tr("提示"),SString::toFormat("累计导入%d张表,共%d条记录!%s",
		m_DbRestore.m_iSaveTabs,m_DbRestore.m_iSaveRows,m_DbRestore.m_iDbErrors==0?"":(SString::toFormat("其中%d批SQL指令失败!",m_DbRestore.m_iDbErrors).data())).data());
}

