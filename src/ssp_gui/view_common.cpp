#include "view_common.h"
#include "ssp_gui.h"

view_common::view_common(QWidget *parent,SString sFunName)
	: CBaseView(parent)
{
	ui.setupUi(this);
	m_refName = /*"ds_" + */sFunName;
	m_fieldlist = new SPtrList<CSsp_DwColumn>;
	InitDataWindow(sFunName);
	SQt::SetTableWidgetStyle(ui.tableWidget);
	ui.tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
	ui.tableWidget->horizontalHeader()->setToolTip(tr("点击列头可自适应宽度"));
	//InitWidget(sFunName);
	m_nPkey = -1;
	m_font_underline.setUnderline(true);

}
view_common::~view_common(void)
{
	delete m_fieldlist;
	//delete m_pPool;
//	delete m_pOper;
	//delete m_dataWindow;
}
void view_common::InitDataWindow(SString sWname)
{
	S_UNUSED(sWname);
	m_dataWindow = GET_DW_CFG;
	//m_pPool = new SDatabasePool<SMySQL>;
	//m_pPool->SetParams("hostaddr=127.0.0.1;port=3306;user=root;password=123321;dbname=db;");
	//m_pPool->CreateDatabasePool(5);
	//m_pOper = new SDatabaseOper();
	//m_pOper->SetDatabasePool((SDatabasePool<SDatabase>*)m_pPool);
	//m_dataWindow->SetDbOper(m_pOper);
	//BOOLCHECKRETURN(m_dataWindow->Load("../conf/sys_datawindow.xml"),tr("加载配置文件失败！"),);
	////BOOLCHECKRETURN(m_pOper->ExecuteSQL("set names gb2312"),tr("执行语句set names gb2312失败！"),);
	////m_dbInter.setDb(m_pOper->GetDatabase());
}
bool view_common::InitWidget(SString sWname)
{
	S_UNUSED(sWname);
	//SDatabase * pdb = m_pOper->GetDatabase();
	SSP_GUI->ShowWaiting();
	ClearUnusefulData(/*pdb*/);
	//m_pOper->ReleaseDatabase(pdb);
	m_tableRow =0;
	m_fieldNamelist.clear();
	m_fieldlist->clear();
	m_vKey.clear();
	ui.tableWidget->clear();
	ui.pushButton_2->setEnabled(false);
	ui.pushButton_3->setEnabled(false);
	ui.pushButton->setEnabled(false);
	SQt::ProcessUiEvents();
	SetCommonToWidget();
	SSP_GUI->HideWaiting();
	return true;
}
SPtrList<CSsp_DwColumn> *view_common::getFieldInfo(SString sWname,SPtrList<CSsp_DwColumn> *cFinfo)
{
	CSsp_Dataset* dbSet=NULL;
	dbSet =  m_dataWindow->SearchDataset(sWname);
	if (dbSet==NULL)
	{
		return m_fieldlist;
	}
	int iCount = dbSet->GetColumnCount();
	for(int i = 0;i<iCount;i++)
	{
		m_fieldlist->append(dbSet->GetColumn(i));
		if (cFinfo!=NULL)
		{
			cFinfo->append(dbSet->GetColumn(i));
		}
	}
	return m_fieldlist;
}
void view_common::SetCommonToWidget()
{
	m_bInsert = true;
	CSsp_Dataset* dbSet=NULL;
	dbSet =  m_dataWindow->SearchDataset(m_refName);
	if (dbSet==NULL)
	{
		SQt::ShowErrorBox(QObject::tr("提示"),QObject::tr("获取数据集失败"));
		return;
	}
	m_tableDesc= m_dataWindow->SearchDataset(m_refName)->GetDesc();

	ui.pushButton->setEnabled(true);
	getFieldInfo(m_refName);

	QStringList lHeader;
	for (int i = 0;i<m_fieldlist->count();i++)
	{
		lHeader<<QString::fromLocal8Bit((*m_fieldlist)[i]->m_sName.data());
		if ((*m_fieldlist)[i]->m_bPKey)
		{
			//m_nPkey = i;
			m_vKey.push_back(i);
		}
	}
	if (!m_vKey.empty())
	{
		m_nPkey=m_vKey[0];
	}
	SString sSql = dbSet->GetSql();
	m_tableName = GetTableFromSql(sSql);
	bool bAutoTrans = dbSet->GetAutoColumnTransform();
	SQt::ProcessUiEvents();
	m_Record =dbSet->GetRecordset();
	SQt::ProcessUiEvents();
	int iRow = m_Record->GetRows();
	m_tableRow = iRow;
	int i,j,iColumn = m_Record->GetColumns();
	for (i = 0;i<iColumn;i++)
	{
		m_fieldNamelist.push_back(m_Record->GetColumnName(i));
	}
	ui.tableWidget->setColumnCount(iColumn);
	ui.tableWidget->setRowCount(iRow);
	ui.tableWidget->setHorizontalHeaderLabels(lHeader);
//  	if(m_fieldlist->count()!=iColumn)
//  		return;
	CSsp_DwColumn **pDwCol = m_fieldlist->count()==0?NULL:(new CSsp_DwColumn*[m_fieldlist->count()]);
	SRecordset **pRef = m_fieldlist->count()==0?NULL:(new SRecordset*[m_fieldlist->count()]);
	bool *bHlink = m_fieldlist->count()==0?NULL:(new bool[m_fieldlist->count()]);
	SString *psTip = m_fieldlist->count()==0?NULL:(new SString[m_fieldlist->count()]);
	SString act;
	SRecord *pRec;
	i=0;
	for(i=0;i<m_fieldlist->count();i++)
	{
		pDwCol[i] = m_fieldlist->at(i);
		if(pDwCol[i] == NULL || pDwCol[i]->m_pRef == NULL)
			pRef[i] = NULL;
		else
		{
			SString sWhere = SString::GetAttributeValue(pDwCol[i]->m_sExtAttr,"where");
			if(sWhere.length()>0)
			{
				//有动态条件
				SString sql = sWhere;
				SString sFldKey;
				int p1,p2;
				p1 = sql.find("{@SESSION:");
				while(p1 >= 0)
				{
					p2 = sql.find("@}",p1);
					if(p2 < 0)
						break;
					sFldKey = sql.mid(p1+10,p2-p1-10);
					sql = sql.left(p1)+SSP_BASE->GetSessionAttributeValue(sFldKey.data())+sql.mid(p2+2);
					p1 = sql.find("{@SESSION:");
				}
				sWhere = sql;
				pDwCol[i]->m_pRef->SetInnerRsIsDirty();
				pRef[i] = pDwCol[i]->m_pRef->GetInnerRecordset(0,sWhere);
			}
			else
				pRef[i] = pDwCol[i]->m_pRef->GetInnerRecordset();
		}
		bHlink[i] = false;
		if(pDwCol[i] != NULL)
		{
			act = SString::GetAttributeValue(pDwCol[i]->m_sExtAttr,"act");
			if(act == "gen_act" || act == "float_wnd" || act == "dialog_wnd")
			{
				psTip[i] = SString::GetAttributeValue(pDwCol[i]->m_sExtAttr,"tip");
				bHlink[i] = true;
			}

		}
	}
	for (i=0;i<iRow;i++)
	{
		if(i%10 == 0)
			SQt::ProcessUiEvents();
		pRec = m_Record->GetRecord(i);
		for (j = 0;j<iColumn;j++)
		{
			if(!(j>=m_fieldlist->count()))
			{
				//pDwCol = m_fieldlist->at(j);
				if(!bAutoTrans && pDwCol[j]->m_sConv!="")
					ui.tableWidget->setItem(i,j,new QTableWidgetItem(pDwCol[j]->GetConvDb2Sys(pRec->GetValue(j)).data()));
				else if (pRef[j] != NULL)
					ui.tableWidget->setItem(i,j,new QTableWidgetItem(showAsRef(pRef[j],pRec->GetValue(j)).data()));
				else
					ui.tableWidget->setItem(i,j,new QTableWidgetItem(pRec->GetValue(j).data()));
			
				if(bHlink[j])
				{
					QTableWidgetItem *pItem = ui.tableWidget->item(i,j);
					pItem->setFont(m_font_underline);
					pItem->setTextColor(Qt::blue);
					pItem->setTextAlignment(Qt::AlignCenter);
					if(psTip[j].length() > 0)
						pItem->setToolTip(psTip[j].data());
				}

			}
		}
	}
	if(pDwCol != NULL)
		delete[] pDwCol;
	if(pRef != NULL)
		delete[] pRef;
	if(bHlink != NULL)
		delete[] bHlink;
	if(psTip != NULL)
		delete[] psTip;
	//ui.tableWidget->setCurrentItem(ui.tableWidget->item(0,0));
	ui.tableWidget->resizeColumnsToContents();
	m_bInsert = false;
}
void view_common::SetToComb(mCombobox * pComb,SRecordset* rs,SString sCurrentKeyValue,int colum)
{
	int iIndex = 0;
	for (int i = 0;i<rs->GetRows();i++)
	{
		SString s = rs->GetValue(i,colum).data();
		pComb->addItem(s.data());
		if (sCurrentKeyValue==s)
		{
			iIndex = i;
		}
	}
	pComb->setCurrentIndex(iIndex);
}
SString view_common::GetFromComb(mCombobox *pComb,SRecordset* rs)
{
	return rs->GetValue(pComb->currentIndex(),0);
}
SString view_common::GetTableFromSql(SString sSql)
{
	SString sql = sSql;
	sql = sql.replace("\r\n"," ");
	sql = sql.replace("\n"," ");
	SString sRight =sql.right(sql.length()- sql.find(" from ",0,false)-6).stripWhiteSpace();
	if (sRight.find(' ')>0)
	{
		sRight = sRight.left(sRight.find(' '));
	}
	return sRight;
}
void view_common::on_pushButton_pressed()
{
	if(m_nPkey < 0)
	{
		SQt::ShowErrorBox(QObject::tr("提示"),QObject::tr("数据集未定义主键，不支持该操作!"));
		return;
	}
	m_bInsert=true;
	int m_row = ui.tableWidget->rowCount();
	int m_colum = ui.tableWidget->columnCount();
	//int m_colum = m_fieldlist->count();
	int cindex=getAddIndex();
	ui.tableWidget->setRowCount(m_row);
	ui.tableWidget->insertRow(m_row);
	if(m_colum<1)
		return;
	for (int j = 0;j<m_colum;j++)
	{
		SString sInitVal = SString::GetAttributeValue((*m_fieldlist)[j]->m_sExtAttr,"init_val");
		if(sInitVal.length() > 0)
		{
			if(sInitVal.left(7).toLower() == "select ")
			{
				//sql
				SString sql = sInitVal;
				SString sFldKey;
				int p1,p2;
				p1 = sql.find("{@SESSION:");
				while(p1 >= 0)
				{
					p2 = sql.find("@}",p1);
					if(p2 < 0)
						break;
					sFldKey = sql.mid(p1+10,p2-p1-10);
					sql = sql.left(p1)+SSP_BASE->GetSessionAttributeValue(sFldKey.data())+sql.mid(p2+2);
					p1 = sql.find("{@SESSION:");
				}
				sInitVal = DB->SelectInto(sql);
			}
		}

		ui.tableWidget->setItem(m_row,j,new QTableWidgetItem(sInitVal.data()));
	}
	if (cindex>=0 && m_nPkey >= 0)
	{
		ui.tableWidget->item(m_row,m_nPkey)->setText(QVariant(cindex).toString());
	}
	SRecordset* pRef;
	for (int j = 0;j<m_fieldlist->count();j++)
	{
		if ((*m_fieldlist)[j]->m_pRef!=NULL)
		{
			pRef  = (*m_fieldlist)[j]->m_pRef->GetInnerRecordsetDirect();
			ui.tableWidget->item(m_row,j)->setText(pRef->GetValue(0,1).data());
		}
	}

	QScrollBar *v_bar = ui.tableWidget->verticalScrollBar();
	if (v_bar!=NULL)
	{
		v_bar->setValue(v_bar->maximumHeight());
	}
	m_bInsert=false;
	ui.tableWidget->setCurrentItem(ui.tableWidget->item(m_row,0));
	ui.pushButton_3->setEnabled(true);
}
void view_common::on_pushButton_2_pressed()
{
	CSsp_Dataset* dbSet=NULL;
	dbSet =  m_dataWindow->SearchDataset(m_refName);
	if (dbSet==NULL)
	{
		SQt::ShowErrorBox(QObject::tr("提示"),QObject::tr("获取数据集失败"));
		return;
	}
	if(m_nPkey < 0)
	{
		SQt::ShowErrorBox(QObject::tr("提示"),QObject::tr("数据集未定义主键，不支持该操作!"));
		return;
	}
	SString sHiddenPK = SString::GetAttributeValue(dbSet->m_sExtAttr,"hidden_pk");
	SString sHiddenWhere;
	if(sHiddenPK.length() > 0)
	{
		int hpk,hpks = SString::GetAttributeCount(sHiddenPK,",");
		SString colname;
		for(hpk=1;hpk<=hpks;hpk++)
		{
			colname = SString::GetIdAttribute(hpk,sHiddenPK);
			if(sHiddenWhere.length() > 0)
				sHiddenWhere += "and";
			sHiddenWhere += SString::toFormat(" %s='%s' ",colname.data(),SSP_GUI->GetSessionAttributeValue(colname.data()).data());
		}
	}
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
	CSsp_DwColumn *pDwCol;
	foreach (const QTableWidgetSelectionRange &qRange, lSelectedRange)
	{
		if (qRange.leftColumn()==0&&qRange.rightColumn()==m_colum-1)//选中了整行
		{
			for(int i = qRange.bottomRow();i>=qRange.topRow();i--)
			{

				if (i<m_tableRow)
				{
					ui.pushButton_3->setEnabled(true);
					m_tableRow--;
					SString sql;
					sql.sprintf("delete from %s where ",m_tableName.data());
					SString sHiddenPK = SString::GetAttributeValue(dbSet->m_sExtAttr,"hidden_pk");
					//MEMO: 增加隐藏主键条件 [2018-9-11 10:40 邵凯田]
					if(sHiddenPK.length() > 0)
					{
						int hpk,hpks = SString::GetAttributeCount(sHiddenPK,",");
						for(hpk=1;hpk<=hpks;hpk++)
						{
							SString colname = SString::GetIdAttribute(hpk,sHiddenPK,",");
							SString colvalue = SSP_GUI->GetSessionAttributeValue(colname.data());
							sql += colname+"='"+colvalue+"' and";							
						}
					}
					for(int pk=0;pk<(int)m_vKey.size();pk++)
					{
						int pkcol = m_vKey[pk];
						SString val;
						pDwCol = (*m_fieldlist)[pkcol];
						if(pDwCol->m_pRef != NULL)
							val=refToValue(pDwCol->m_pRef->GetInnerRecordsetDirect(),ui.tableWidget->item(i,pkcol)->text().toStdString().data());
						else if( pDwCol->m_sConv!="")
							val=pDwCol->GetConvSys2Db(ui.tableWidget->item(i,pkcol)->text().toStdString().data());
						else
							val=ui.tableWidget->item(i,pkcol)->text().toStdString().data();
						sql += SString::toFormat("%s %s='%s' ",pk==0?"":"and",
							m_Record->GetColumnName(pkcol).data(),val.data());
					}
					m_sqlRecord.addSql(sql,1);
				}
				ui.tableWidget->removeRow(i);
			}
		}
	}
	if (m_sqlRecord.isEmpty())
	{
		ui.pushButton_3->setEnabled(false);
	}
}
void view_common::on_pushButton_3_pressed()
{
	CSsp_Dataset* dbSet=NULL;
	dbSet =  m_dataWindow->SearchDataset(m_refName);
	if (dbSet==NULL)
	{
		SQt::ShowErrorBox(QObject::tr("提示"),QObject::tr("获取数据集失败"));
		return;
	}
	//bool bAutoTrans = dbSet->GetAutoColumnTransform();
	//存入插入语句
	SString sSql_left = SString::toFormat("insert into %s(",m_tableName.data());
	SString sHiddenPK = SString::GetAttributeValue(dbSet->m_sExtAttr,"hidden_pk");
	bool bBeg=false;
	if(sHiddenPK.length() > 0)
	{
		bBeg = true;
		sSql_left += sHiddenPK;
	}
	for (int i = 0;i<(int)m_fieldNamelist.size();i++)
	{
		//sSql_left+="`"+m_fieldNamelist[i]+"`"+",";
		CSsp_DwColumn *pCol = dbSet->GetColumn(i);
		if(pCol == NULL || SString::GetAttributeValue(pCol->m_sExtAttr,"edit") == "0")
			continue;
		if(bBeg == false)
			bBeg = true;
		else
			sSql_left += ",";
		sSql_left+=m_fieldNamelist[i];
	}
	sSql_left+=") values(";
	SString sSql = sSql_left;
	for(int i = m_tableRow;i<ui.tableWidget->rowCount();i++)
	{
		bBeg=false;
		if(sHiddenPK.length() > 0)
		{
			int hpk,hpks = SString::GetAttributeCount(sHiddenPK,",");
			for(hpk=1;hpk<=hpks;hpk++)
			{
				SString colname = SString::GetIdAttribute(hpk,sHiddenPK,",");
				SString colvalue = SSP_GUI->GetSessionAttributeValue(colname.data());
				if(!bBeg)
					bBeg = true;
				else
					sSql += ",";
				sSql += SString::toFormat("'%s'",colvalue.data());
			}
		}
		for (int j = 0;j<ui.tableWidget->columnCount();j++)
		{
			CSsp_DwColumn *pCol = dbSet->GetColumn(j);
			if(pCol == NULL || SString::GetAttributeValue(pCol->m_sExtAttr,"edit") == "0")
				continue;
			if(!bBeg)
				bBeg = true;
			else
				sSql += ",";

			if (ui.tableWidget->item(i,j)->text()==""&&(*m_fieldlist)[j]->m_pRef==NULL)
			{
				if (!canNUll(j))
				{
					QMessageBox::information(NULL,"tip",ui.tableWidget->horizontalHeaderItem(j)->text()+tr("不能为空,保存失败!"));
					return;
				}
				sSql+="NULL";
				continue;
			}
			if ((*m_fieldlist)[j]->m_iType==0)
			{
				sSql+="'";
			}
			if ((*m_fieldlist)[j]->m_pRef!=NULL)
			{
				//mCombobox * pComb = NULL;
				//pComb = (mCombobox *)ui.tableWidget->cellWidget(i,j);
				//sSql+=GetFromComb(pComb,(*m_fieldlist)[j]->m_pRef->GetRecordset());
				sSql+=refToValue((*m_fieldlist)[j]->m_pRef->GetInnerRecordsetDirect(),ui.tableWidget->item(i,j)->text().toStdString().data());
			}else
			{
				if(/*!bAutoTrans && */(*m_fieldlist)[j]->m_sConv!="")
					sSql+=(*m_fieldlist)[j]->GetConvSys2Db(ui.tableWidget->item(i,j)->text().toStdString().data());
				else
					sSql+=ui.tableWidget->item(i,j)->text().toStdString();
			}
			if ((*m_fieldlist)[j]->m_iType==0)
			{
				sSql+="'";
			}
		}
		sSql+=")";
		//sSql+="'"+ui.tableWidget.item(i,ui.tableWidget->columnCount()-1)->text().toStdString()+"')";
		m_sqlRecord.addSql(sSql,0);
		sSql = sSql_left;
	}

	//SDatabase * pDb = m_pOper->GetDatabase();
	
	if (!m_sqlRecord.exec(/*pDb*/))
	{
		m_bSave=true;
		ui.pushButton_3->setEnabled(false);
		InitWidget(m_refName);
	}
	//m_pOper->ReleaseDatabase(pDb);
	m_tableRow = ui.tableWidget->rowCount();
	m_bSave=true;
	ui.pushButton_3->setEnabled(false);
}
void view_common::on_pushButton_4_clicked()
{
	SQt::TablePrint(m_tableDesc,ui.tableWidget);
	//int i = 10;
}
void view_common::on_pushButton_5_clicked()
{
	SQt::TableExort(m_tableDesc,ui.tableWidget);
}
void view_common::on_tableWidget_itemChanged(QTableWidgetItem * item)
{
	if(m_nPkey < 0)
		return;
	CSsp_Dataset* dbSet=NULL;
	dbSet =  m_dataWindow->SearchDataset(m_refName);
	if (dbSet==NULL)
	{
		SQt::ShowErrorBox(QObject::tr("提示"),QObject::tr("获取数据集失败"));
		return;
	}
	//bool bAutoTrans = dbSet->GetAutoColumnTransform();

	ui.tableWidget->blockSignals(true);
	QList<int> keyCol;
	if (m_bInsert)
	{
		ui.tableWidget->blockSignals(false);
		return;
	}
	int column = item->column();

	if (!canNUll(column))
	{
		if (item->text()=="")
		{
			QMessageBox::information(NULL,"tip",ui.tableWidget->horizontalHeaderItem(column)->text()+tr("不能为空!"));
			m_bInsert=true;
			item->setText(m_pre.data());
			m_bInsert=false;
			ui.tableWidget->blockSignals(false);
			return;
		}
	}
	for(int j=0;j<ui.tableWidget->columnCount();j++)
	{
		if(isUnique(j))
		{
			keyCol.append(j);
		}
	}

	if (isUnique(column)&&ui.tableWidget->rowCount()>1)
	{
		bool isAllSame=true;
		for(int j=0;j<ui.tableWidget->columnCount();j++)
		{
			if(isUnique(j))
			{
				
				QString text=ui.tableWidget->item(item->row(),j)->text();
				for(int i = 0;i<ui.tableWidget->rowCount();i++)
				{
					if (ui.tableWidget->item(i,/*m_nPkey*/j)->text()!=text)
					{
						isAllSame=false;
					}
				}
			}

		}
		if(isAllSame==true)
		{
			int i_ret = QMessageBox::warning(this,tr("提示"),ui.tableWidget->horizontalHeaderItem(column)->text()+tr("必须唯一！"),QMessageBox::Yes);
			if (i_ret==QMessageBox::Yes)
			{
				m_bInsert=true;
				item->setText(m_pre.data());
				m_bInsert=false;
				ui.tableWidget->blockSignals(false);
				return;
			}
		}

	}
	if (item->row()>=m_tableRow)
	{
		ui.tableWidget->blockSignals(false);
		return;
	}
	SString sHiddenPK = SString::GetAttributeValue(dbSet->m_sExtAttr,"hidden_pk");
	SString sHiddenWhere;
	if(sHiddenPK.length() > 0)
	{
		int hpk,hpks = SString::GetAttributeCount(sHiddenPK,",");
		SString colname;
		for(hpk=1;hpk<=hpks;hpk++)
		{
			colname = SString::GetIdAttribute(hpk,sHiddenPK,",");
			if(sHiddenWhere.length() > 0)
				sHiddenWhere += "and";
				sHiddenWhere += SString::toFormat(" %s='%s' ",colname.data(),SSP_GUI->GetSessionAttributeValue(colname.data()).data());
		}
	}
	SString stext=item->text().toStdString().data();
	SString sSql = SString::toFormat("update %s set %s=",m_tableName.data(),m_fieldNamelist[column].data());
	if (item->text()=="")
	{
		sSql+= "NULL";
	}else
	{
		if ((*m_fieldlist)[column]->m_pRef!=NULL)
		{
			stext=(*m_fieldlist)[column]->m_pRef->GetInnerRecordsetDirect()->GetValue(m_comboChangeIndex,0);
		}
		if ((*m_fieldlist)[column]->m_iType==0)
		{
			if(/*!bAutoTrans && */(*m_fieldlist)[column]->m_sConv!="")
				stext=(*m_fieldlist)[column]->GetConvSys2Db(stext);
			sSql+="'"+stext+"'";
		}else
		{
			if(/*!bAutoTrans && */(*m_fieldlist)[column]->m_sConv!="")
				stext=(*m_fieldlist)[column]->GetConvSys2Db(stext);
			sSql+=stext;
		}
	}
	SString sKey;
	SString sKeyvalue;
	sSql+=" where 1=1";
	foreach(int m_nPkey,keyCol)
	{
		if (item->column()==m_nPkey)
		{
			sKeyvalue=m_pre;
		}else
		{
			sKeyvalue=ui.tableWidget->item(item->row(),m_nPkey)->text().toStdString();
		}
		if(/*!bAutoTrans && */(*m_fieldlist)[m_nPkey]->m_sConv!="")
			sKeyvalue=(*m_fieldlist)[m_nPkey]->GetConvSys2Db(sKeyvalue);

		if((*m_fieldlist)[m_nPkey]->m_pRef!=NULL)
		{
			SRecordset *tmpset=(*m_fieldlist)[m_nPkey]->m_pRef->GetInnerRecordsetDirect();
			for(int k=0;k<tmpset->GetRows();k++)
			{
				if(sKeyvalue==tmpset->GetValue(k,1))
				{
					sKeyvalue=tmpset->GetValue(k,0);
					break;
				}
			}
		}
		if ((*m_fieldlist)[m_nPkey]->m_iType==0)
		{
			sKey="'"+ sKeyvalue+"'";
		}else
		{
			sKey= sKeyvalue;
		}
		sSql+=" and "+m_fieldNamelist[m_nPkey]+"="+sKey;
	}
	if(sHiddenWhere.length() > 0)
		sSql += " and"+sHiddenWhere;
	m_sqlRecord.addSql(sSql,2);
	ui.pushButton_3->setEnabled(true);
	ui.tableWidget->blockSignals(false);
}
void view_common::on_tableWidget_itemDoubleClicked(QTableWidgetItem * item)
{
	if(m_nPkey < 0)
		return;
	if(item->column() < 0)
		return;
	m_pre=item->text().toStdString();
	CSsp_DwColumn *pDwCol = m_fieldlist->at(item->column());
	if(pDwCol == NULL || SString::GetAttributeValue(pDwCol->m_sExtAttr,"edit") == "0")
		return;//MEMO: 禁止编辑 [2015-12-30 9:25 邵凯田]
	//int num1=(*m_fieldlist)[m_nPkey]->m_iType;
	if ((*m_fieldlist)[m_nPkey]->m_iType!=2||m_nPkey!=item->column())
	{
		ui.tableWidget->editItem(item);
	}
}
void view_common::on_tableWidget_itemClicked(QTableWidgetItem * item)
{
	if(item->row() < 0 || item->column() < 0)
		return;
	CSsp_Dataset* dbSet=NULL;
	dbSet =  m_dataWindow->SearchDataset(m_refName);
	if (dbSet==NULL)
	{
		SQt::ShowErrorBox(QObject::tr("提示"),QObject::tr("获取数据集失败"));
		return;
	}

	CSsp_DwColumn *pDwCol = m_fieldlist->at(item->column());
	if(pDwCol == NULL)
		return;
	SString act = SString::GetAttributeValue(pDwCol->m_sExtAttr,"act");
	if(act.length() == 0)
		return;
	if(act != "gen_act" && act != "float_wnd" && act != "dialog_wnd")
		return;
	SString sExtAttr = pDwCol->m_sExtAttr;
	int p1,p2;
	int col;
	SString colvalue;
	while((p1=sExtAttr.find("{@COL:")) >= 0)
	{
		p1+=6;
		p2 = sExtAttr.find("@}",p1);
		if(p2 < p1)
			break;
		col = sExtAttr.mid(p1,p2-p1).toInt();
		
		if(col > 0 && col <= m_fieldlist->count())
		{
			colvalue = dbSet->GetInnerRecordsetDirect()->GetValue(item->row(),col-1);
			sExtAttr = sExtAttr.left(p1-6)+colvalue+sExtAttr.mid(p2+2);
		}
		else
			break;
	}
	SSP_GUI->SetSessionAttributeValues(sExtAttr);

	if(act == "gen_act")//通用动作
	{
		int sn = SString::GetAttributeValueI(pDwCol->m_sExtAttr,"sn");
		SSP_GUI->RunGeneralAction(sn);
		OnPreShow();
	}
	else if(act == "float_wnd")//浮动组态窗口
	{
		int sn = SString::GetAttributeValueI(pDwCol->m_sExtAttr,"sn");
		SSP_GUI->OpenFloatWnd(sn,SString::GetAttributeValueI(pDwCol->m_sExtAttr,"w"),SString::GetAttributeValueI(pDwCol->m_sExtAttr,"h"),
			SString::GetAttributeValueI(pDwCol->m_sExtAttr,"alpha"),SString::GetAttributeValue(pDwCol->m_sExtAttr,"title"));
		OnPreShow();
	}
	else if(act == "dialog_wnd")//弹出组态窗口
	{
		int sn = SString::GetAttributeValueI(pDwCol->m_sExtAttr,"sn");
		SSP_GUI->OpenDialogWnd(sn,SString::GetAttributeValueI(pDwCol->m_sExtAttr,"w"),SString::GetAttributeValueI(pDwCol->m_sExtAttr,"h"),
			SString::GetAttributeValueI(pDwCol->m_sExtAttr,"alpha"),SString::GetAttributeValue(pDwCol->m_sExtAttr,"title"));
		OnPreShow();
	}
}

void view_common::on_tableWidget_itemSelectionChanged()
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

void view_common::on_tableWidget_currentItemChanged( QTableWidgetItem * current, QTableWidgetItem * previous )
{
	if(current==NULL)
		return;
	if (m_fieldlist->isEmpty())
	{
		return;
	}
	for (int j = 0;j<ui.tableWidget->columnCount();j++)
	{		
		if ((*m_fieldlist)[j]->m_pRef!=NULL)
		{
			if (previous!=NULL)
			{
				ui.tableWidget->removeCellWidget(previous->row(),j);
			}
			SRecordset *pRef  = (*m_fieldlist)[j]->m_pRef->GetInnerRecordsetDirect();
			mCombobox * pComb = new mCombobox();
			pComb->installEventFilter(this);
			pComb->setRowAndColumn(ui.tableWidget->item(current->row(),j),j);
			pComb->setInsertPolicy(mCombobox::InsertAtBottom);
			SetToComb(pComb,pRef,ui.tableWidget->item(current->row(),j)->text().toStdString().data(),1);
			ui.tableWidget->setCellWidget(current->row(),j,pComb);
			m_comboChangeIndex=pComb->currentIndex();
			ui.tableWidget->item(current->row(),j)->setText(pComb->currentText());
			QObject::connect(pComb,SIGNAL(whitch(QTableWidgetItem*,int)),this,SLOT(oneCellChanged(QTableWidgetItem*,int)));
			pComb->setFocusProxy(this);
		}
	}
}
bool view_common::eventFilter(QObject *obj, QEvent *event)
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
void view_common::oneCellChanged(QTableWidgetItem* item,int column)
{
	if (m_bInsert)
		return;
	//if (item->row()>=m_tableRow)
	//{
	//	return;
	//}
	mCombobox * pComb = (mCombobox *)sender();
	SString sValue = GetFromComb(pComb,(*m_fieldlist)[column]->m_pRef->GetInnerRecordsetDirect());
	//item->setText(sValue.data());
	m_comboChangeIndex=pComb->currentIndex();
	m_pre=item->text().toStdString().data();
	item->setText(pComb->currentText());
}

int view_common::getAddIndex()
{
	int iRet=0;
	if(m_nPkey < 0 || (*m_fieldlist)[m_nPkey]==NULL)
		return -1;
	if ((*m_fieldlist)[m_nPkey]->m_iType==2)
	{
		for (int i=0;i<ui.tableWidget->rowCount();i++)
		{
			if (ui.tableWidget->item(i,m_nPkey)->text().toInt()>iRet)
			{
				iRet=ui.tableWidget->item(i,m_nPkey)->text().toInt();
			}
		}
		iRet++;
		return iRet;
	}
	return -1;
}


SString view_common::showAsRef(SRecordset* rs,SString sCurrentKeyValue)
{
	SString sret=sCurrentKeyValue;
	for (int i = 0;i<rs->GetRows();i++)
	{
		if (sCurrentKeyValue==rs->GetValue(i,0))
		{
			sret = rs->GetValue(i,1).data();
		}
	}
	return sret;
}
SString view_common::refToValue(SRecordset* rs,SString sref)
{
	SString sret=sref;
	for (int i = 0;i<rs->GetRows();i++)
	{
		if (sref==rs->GetValue(i,1))
		{
			sret = rs->GetValue(i,0).data();
		}
	}
	return sret;
}
void view_common::OnPreShow()
{
	InitWidget(m_refName);
}
void view_common::OnPreHide()
{
	if (!ui.pushButton_3->isEnabledTo(this))
		return;
	int i_ret = QMessageBox::warning(this,tr("提示"),tr("是否保存？"),QMessageBox::Yes,QMessageBox::No);
	if (i_ret==QMessageBox::Yes)
	{
		on_pushButton_3_pressed();
	}
	else
	{
		m_sqlRecord.clear();
	}
}
void view_common::OnAftHide()
{
	//QMessageBox::warning(this,tr("提示！"),tr("是否保存？"),QMessageBox::Yes,QMessageBox::No);
}
bool view_common::isPkey(int column)
{
	for(int i=0;i<(int)m_vKey.size();i++)
	{
		if (column==m_vKey[i])
		{
			return true;
		}
	}
	return false;
}
bool view_common::isUnique(int column)
{
	return (*m_fieldlist)[column]->m_bUniqueKey;
}
bool view_common::canNUll(int column)
{
	return (*m_fieldlist)[column]->m_bIsNull;
}