#include "dbInteraction.h"


dbInteraction::dbInteraction(SDatabaseOper * pDb)
{
	m_db = pDb;
	//m_tree = new pcell_CTree<SString>;
	//m_tempTree = new pcell_CTree<SString>;
}


dbInteraction::~dbInteraction(void)
{
	if (m_db!=NULL)
	{
		//delete m_db;
		m_db=NULL;
	}
}

void dbInteraction::GetChildren(SString sTable,SString sField,SString sFField,SString sValue,QTreeWidgetItem*&item)
{
	SString sData = sValue;
	SRecordset rs;
	m_db->Retrieve(SString::toFormat("select * from %s where %s = '%s'",sTable.data(),sField.data(),sValue.data()),rs);
	addData(rs,sData,item);
	rs.clear();
	int iRet;
	SString sSql= SString::toFormat("select %s,%s from %s where %s='%s' order by Idx",sField.data(),sFField.data(),sTable.data(),sFField.data(),sValue.data());
	iRet = m_db->Retrieve(sSql,rs);
	if (iRet>0)
	{
		for (int i = 0;i<rs.GetRows();i++)
		{
			//pcell_CTree<SString> *pcTree = new pcell_CTree<SString>;
			QTreeWidgetItem * pitem=new QTreeWidgetItem();
			//tTemp->addChild(pcTree);
			item->addChild(pitem);
			GetChildren(sTable,sField,sFField,rs.GetValue(i,0),pitem);
		}

	}
}

mCombobox * dbInteraction::getRefNum()
{
	SString sSql="select wnd_sn,wnd_name from t_ssp_uicfg_wnd where wnd_name is not null order by cls_name,wnd_name";
	SRecordset rs;
	mCombobox *pComb=new mCombobox(0,1);
	pComb->addItem("");
	if (m_db->Retrieve(sSql,rs)>0)
	{
		for(int i=0;i<rs.GetRows();i++)
		{
			pComb->addItem(rs.GetValue(i,1).data());
		}
		//pComb->setMinimumHeight(23);
	}
	return pComb;
}
mCombobox * dbInteraction::getReportNum()
{
	SString sSql="select report_no,report_name from t_ssp_report_info where report_name is not null order by report_no";
	SRecordset rs;
	mCombobox *pComb=new mCombobox(0,1);
	pComb->addItem("");
	if (m_db->Retrieve(sSql,rs)>0)
	{
		for(int i=0; i < rs.GetRows();i++)
		{
			pComb->addItem(rs.GetValue(i,1).data());
		}
		//pComb->setMaximumHeight(50);
	}
	return pComb;
}
void dbInteraction::initUicfgAndReportRef()
{
	SString sSql ="select report_no,report_name from t_ssp_report_info order by report_no";
	SRecordset rs;
	m_reportInfoMap.clear();
	if (m_db->Retrieve(sSql,rs) > 0)
	{
		for(int i=0; i < rs.GetRows();i++)
		{
			m_reportInfoMap[rs.GetValue(i,0).data()] = rs.GetValue(i,1).data();
		}
	}

	sSql="select wnd_sn,wnd_name from t_ssp_uicfg_wnd order by cls_name,wnd_name";
	m_uicfgWndMap.clear();
	rs.clear();
	if (m_db->Retrieve(sSql,rs)>0)
	{
		for(int i=0;i<rs.GetRows();i++)
		{
			m_uicfgWndMap[rs.GetValue(i,0).data()] = rs.GetValue(i,1).data();
		}
	}
}
void dbInteraction::setRefIndex(mCombobox *pComb,QString val)
{
	QString sCombValue;
	for (int i=0;i<pComb->count();i++)
	{
		sCombValue=pComb->itemText(i);
		//cmpValue=sCombValue.left(sCombValue.indexOf('-'));
		if (sCombValue==val)
		{
			pComb->setCurrentIndex(i);
			return;
		}
	}
	pComb->setCurrentIndex(-1);
}