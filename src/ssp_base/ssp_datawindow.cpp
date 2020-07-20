#include "ssp_datawindow.h"
#include "ssp_base.h"

//////////////////////////////////////////////////////////////////////////
// 名    称:  CSsp_DwColumn
// 作    者:  邵凯田
// 创建时间:  2015-7-14 9:43
// 描    述:  数据窗口列定义类
//////////////////////////////////////////////////////////////////////////
CSsp_DwColumn::CSsp_DwColumn()
{
	m_pRef = NULL;
	m_bPKey = false;
	m_bIsNull = true;
	m_bUniqueKey = false;
	m_pMdbField = NULL;
}

CSsp_DwColumn::CSsp_DwColumn(SBaseConfig *pColCfg)
{
	m_pMdbField = NULL;
	if(pColCfg->GetAttribute("pkey").compareNoCase("true") == 0)
		m_bPKey = true;
	else
		m_bPKey = false;
	if(pColCfg->GetAttribute("isnull").compareNoCase("false") == 0)
		m_bIsNull = false;
	else
		m_bIsNull = true;
	if(pColCfg->GetAttribute("ukey").compareNoCase("true") == 0)
		m_bUniqueKey = true;
	else
		m_bUniqueKey = false;
	if(m_bPKey == true)
	{
		m_bIsNull = false;//主键时必须为非空
		m_bUniqueKey = true;//主键必须为唯一键
	}
	if(m_bUniqueKey == true)
	{
		m_bIsNull = false;//主键时必须为非空
	}

	m_sName = pColCfg->GetAttribute("name");//列名称
	m_sConv = pColCfg->GetAttribute("conv");//列名称
	m_sExtAttr = pColCfg->GetAttribute("ext_attr");//扩展属性
	m_iWidth = pColCfg->GetAttributeI("width");//列显示宽度
	SString sType = pColCfg->GetAttribute("type");//列数值类型，0:字符串;1:整数;2:浮点数;
	sType.toLower();
	if(sType == "int")
		m_iType = 1;
	else if(sType == "float")
		m_iType = 2;
	else
		m_iType = 0;//string
	int i,cnt = pColCfg->GetAttributeCount();
	for(i=0;i<cnt;i++)
	{
		sType = pColCfg->GetAttributeName(i);
		if(sType == "name" || sType == "width" || sType == "type" || sType == "ref")
			continue;
		m_sExtAttr += sType+"="+pColCfg->GetAttribute(sType)+";";
	}
	m_pRef = NULL;
	//CSsp_Reference *m_pRef;//关联的引用,NULL表示不关联引用
}

CSsp_DwColumn::~CSsp_DwColumn()
{

}

//////////////////////////////////////////////////////////////////////////
// 描    述:  将当前对象克隆一份并返回指针
// 作    者:  邵凯田
// 创建时间:  2015-11-13 15:43
// 参数说明:  void
// 返 回 值:  CSsp_DwColumn*
//////////////////////////////////////////////////////////////////////////
CSsp_DwColumn* CSsp_DwColumn::Clone()
{
	CSsp_DwColumn *pNewCol = new CSsp_DwColumn();
	pNewCol->m_iIdx = m_iIdx;
	pNewCol->m_sName = m_sName;
	pNewCol->m_sDbName = m_sDbName;
	pNewCol->m_sConv = m_sConv;
	pNewCol->m_pMdbField = m_pMdbField;
	pNewCol->m_iWidth = m_iWidth;
	pNewCol->m_iType = m_iType;
	pNewCol->m_bPKey = m_bPKey;
	pNewCol->m_bIsNull = m_bIsNull;
	pNewCol->m_bUniqueKey = m_bUniqueKey;
	pNewCol->m_iValLen = m_iValLen;
	pNewCol->m_iValDecLen = m_iValDecLen;
	pNewCol->m_sExtAttr = m_sExtAttr;
	pNewCol->m_pRef = m_pRef;
	return pNewCol;
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  将指定的数据库值转换成为系统可视化值
// 作    者:  邵凯田
// 创建时间:  2015-11-9 16:13
// 参数说明:  @sValue为可视值
// 返 回 值:  SString
//////////////////////////////////////////////////////////////////////////
SString CSsp_DwColumn::GetConvDb2Sys(SString sValue)
{
	SString sRet;
	if(m_sConv == "soc2time")
		sRet = SDateTime::makeDateTime((time_t)sValue.toInt()).toString("yyyy-MM-dd hh:mm:ss");
	else if(m_sConv == "soc2time2")
		sRet = SDateTime::makeDateTime((time_t)sValue.toInt()).toString("yyyy年MM月dd日 hh时mm分ss秒");
	else if(m_sConv == "soc2time3")
		sRet = SDateTime::makeDateTime((time_t)sValue.toInt()).toString("hh:mm:ss");
	else if(m_sConv == "soc2time4")
		sRet = SDateTime::makeDateTime((time_t)sValue.toInt()).toString("hh时mm分ss秒");
	else if(m_sConv == "soc2date")
		sRet = SDateTime::makeDateTime((time_t)sValue.toInt()).toString("yyyy-MM-dd");
	else if(m_sConv == "soc2date2")
		sRet = SDateTime::makeDateTime((time_t)sValue.toInt()).toString("yyyy年MM月dd日");
	else if(m_sConv == "soc2now")
	{
		int now = (int)SDateTime::getNowSoc();
		int sep = now - sValue.toInt();
		if(sep < 60)
			sRet.sprintf("%ds",sep);
		else if(sep < 3600)
			sRet.sprintf("%dm:%ds",sep/60,sep%60);
		else if(sep < 3600*24)
			sRet.sprintf("%dh:%dm:%ds",sep/3600,(sep%3600)/60,(sep%3600)%60);
		else
			sRet.sprintf("%d天%d小时",sep/3600/24,(sep%(3600*24))/3600);
	}
	else if(m_sConv == "int2intk")
		sRet = SString::toBytesString(sValue.toInt());
	else if(m_sConv == "int2thousands")
	{
		while(sValue.length() > 3)
		{
			sRet += ","+sValue.right(3);
			sValue = sValue.left(sValue.length()-3);
		}
		sRet = sValue+sRet;
	}
	else if(m_sConv == "int2ip")
	{
		DWORD ip = sValue.toUInt();
		BYTE *pIp = (BYTE*)&ip;
		sRet.sprintf("%d.%d.%d.%d",pIp[0],pIp[1],pIp[2],pIp[3]);
	}
	return sRet;
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  将指定的系统可视化值转换成为数据库值
// 作    者:  邵凯田
// 创建时间:  2015-11-9 16:13
// 参数说明:  @sValue为可视值
// 返 回 值:  SString
//////////////////////////////////////////////////////////////////////////
SString CSsp_DwColumn::GetConvSys2Db(SString sValue)
{
	SString sRet;
	if(m_sConv == "soc2time")
		sRet = SString::toString(SDateTime::makeDateTime("yyyy-MM-dd hh:mm:ss",sValue).soc());
	else if(m_sConv == "soc2time2")
		sRet = SString::toString(SDateTime::makeDateTime("yyyy年MM月dd日 hh时mm分ss秒",sValue).soc());
	else if(m_sConv == "soc2time3")
		sRet = SString::toString(SDateTime::makeDateTime("hh:mm:ss",SDateTime::currentDateTime().toString("yyyy-MM-dd ")+sValue).soc());
	else if(m_sConv == "soc2time4")
		sRet = SString::toString(SDateTime::makeDateTime("hh时mm分ss秒",SDateTime::currentDateTime().toString("yyyy年MM月dd日 ")+sValue).soc());
	else if(m_sConv == "soc2date")
		sRet = SString::toString(SDateTime::makeDateTime("yyyy-MM-dd",sValue).soc());
	else if(m_sConv == "soc2date2")
		sRet = SString::toString(SDateTime::makeDateTime("yyyy年MM月dd日",sValue).soc());
	else if(m_sConv == "soc2now")
	{
		sRet = sValue;
		//int now = (int)SDateTime::getNowSoc();
		//sRet = SString::toString(SDateTime::makeDateTime("yyyy年MM月dd日",sValue).soc());
	}
	else if(m_sConv == "int2intk")
	{
		float f = sValue.toFloat();
		char ch = '\0';
		if(sValue.length() > 0)
			ch = sValue.at(sValue.length()-1);
		switch(ch)
		{
		case 'K':
			f *= 1024;
			break;
		case 'M':
			f *= 1048576;
			break;
		case 'G':
			f *= 1073741824;
			break;
		}
		sRet = SString::toString((int)f);
	}
	else if(m_sConv == "int2thousands")
	{
		sRet = sValue;
		sRet.replace((char*)",",(char*)"");
	}
	else if(m_sConv == "int2ip")
	{
		sRet = SString::toString((int)inet_addr(sValue.data()));
	}
	return sRet;
}

SPtrList<CSsp_Dataset> g_ssp_datasets;

//////////////////////////////////////////////////////////////////////////
// 名    称:  CSsp_Datawindow
// 作    者:  邵凯田
// 创建时间:  2015-7-14 9:42
// 描    述:  数据窗口类
//////////////////////////////////////////////////////////////////////////
CSsp_Dataset::CSsp_Dataset()
{
	m_bRef = false;
	m_tRsUpdateSOC = 0;
	m_Callbacks.setAutoDelete(true);
	m_Callbacks.setShared(true);
	m_MdbFields.setAutoDelete(true);
	m_Columns.setAutoDelete(true);
	m_DwConditons.setAutoDelete(true);
	m_bInnerRsIsDirty = true;
	m_pMdbTrgClient = NULL;
	m_bWithAggregate = false;
	m_iTransformType = 0;
	m_iCrossRefColumnNo = -1;
	m_iCrossDataColumnNo = -1;
	m_bCrossColumnProcessed = false;
	m_bAutoColumnTransform = false;
	g_ssp_datasets.append(this);
}


CSsp_Dataset::~CSsp_Dataset()
{
	ReleaseMdb();
	g_ssp_datasets.remove(this);
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  根据当前数据集克隆一个新的数据集对象
// 作    者:  邵凯田
// 创建时间:  2015-11-13 15:31
// 参数说明:  void
// 返 回 值:  CSsp_Dataset*
//////////////////////////////////////////////////////////////////////////
CSsp_Dataset* CSsp_Dataset::Clone()
{
	CSsp_Dataset* pNewDs = new CSsp_Dataset();
	pNewDs->m_sName = m_sName;
	pNewDs->m_sDesc = m_sDesc;
	pNewDs->m_sSql = m_sSql;
	pNewDs->m_iType = m_iType;
	pNewDs->m_sTableName = m_sTableName;
	pNewDs->m_pDataWindowMgr = m_pDataWindowMgr;
	pNewDs->m_bRef = m_bRef;
	pNewDs->m_iTransformType = m_iTransformType;
	pNewDs->m_sCrossRefName = m_sCrossRefName;
	pNewDs->m_iCrossRefColumnNo = m_iCrossRefColumnNo;
	pNewDs->m_iCrossDataColumnNo = m_iCrossDataColumnNo;
	pNewDs->m_bCrossColumnProcessed = m_bCrossColumnProcessed;
	pNewDs->m_iCrossRawColumnCnt = m_iCrossRawColumnCnt;
	pNewDs->m_bAutoColumnTransform = m_bAutoColumnTransform;
	pNewDs->m_pMdbTrgClient = m_pMdbTrgClient;
	pNewDs->m_bWithAggregate = m_bWithAggregate;
	
	unsigned long pos=0;
	CSsp_DwColumn *pNewCol;
	CSsp_DwColumn *pCol = m_Columns.FetchFirst(pos);
	while(pCol)
	{
		pNewCol = pCol->Clone();
		pNewDs->m_Columns.append(pNewCol);
		if(m_PrimKeyColumns.exist(pCol))
			pNewDs->m_PrimKeyColumns.append(pNewCol);
		pCol = m_Columns.FetchNext(pos);
	}
	stuDwCondition *pNewCon,*pCon = m_DwConditons.FetchFirst(pos);
	while(pCon)
	{
		pNewCon = new stuDwCondition();
		pNewCon->iConditionSn = pCon->iConditionSn;
		pNewCon->sCmpValue = pCon->sCmpValue;
		pNewDs->m_DwConditons.append(pNewCon);
		pCon = m_DwConditons.FetchNext(pos);
	}
	if(pNewDs->m_iType == 1)
		pNewDs->InitMdb();
	return pNewDs;
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  根据会话参数修复动态条件，修改本对象的SQL语言
// 作    者:  邵凯田
// 创建时间:  2015-11-13 16:02
// 参数说明:  void
// 返 回 值:  void
//////////////////////////////////////////////////////////////////////////
void CSsp_Dataset::PrepareDynamicCondition()
{
	SString sql = m_sSql;
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
	m_sSql = sql;
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  取记录集指针
// 作    者:  邵凯田
// 创建时间:  2015-7-14 11:57
// 参数说明:  @pRs表示目标记录集,NULL表示不提供目标记录集（直接使用对象内置记录值，不保证线程安全性）；
//         :  @sWhere表示替换条件
// 返 回 值:  SRecrodset*,NULL表示失败
//////////////////////////////////////////////////////////////////////////
SRecordset* CSsp_Dataset::GetRecordset(SRecordset *pRs,SString sWhere)
{
	if(m_iType == 0)//fixed
		return &m_Recordset;
	SDatabaseOper *pDbOper = NULL;
	SString sFldKey,sSubWhere;
	SString sql = m_sSql;
	int p1,p2,p3;
	sql.replace((char*)"{$Where$}",sWhere);

	if(sql.find("count(") > 0 || sql.find("sum(")>0 || sql.find("min(")>0 || sql.find("max(")>0|| sql.find("avg(")>0 )
		m_bWithAggregate = true;

	//处理动态会话属性条件 {@SESSION:sub_no@}
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

	//MEMO: 根据动态条件重新组织SQL条件字符串 [2015-10-29 11:34 邵凯田]
	//{ and type [@1] }
	int iConSn = 1;
	stuDwCondition *pCondition;
	while(1)
	{
		sFldKey.sprintf("[@%d]",iConSn);
		p2 = sql.find(sFldKey);
		if(p2 < 0)
			break;
		while(p2 > 0)
		{
			p1 = sql.findRev('{',p2);
			p3 = sql.find("}",p2);
			//是否该条件已启用?
			pCondition = SearchConditon(iConSn);
			if(pCondition == NULL)
			{
				if(p1 < 0 || p3 < 0)
				{
					p2 = sql.find(sFldKey,p2+1);
					continue;
				}
				//放弃该条件
				sql = sql.left(p1)+sql.right(sql.length()-p3-1);
			}
			else
			{
				if(p1 < 0 || p3 < 0)
				{
					//没有{}
					sql = sql.left(p2)+pCondition->sCmpValue+sql.right(sql.length()-(p2+sFldKey.length()));
				}
				else
				{
					sSubWhere = sql.mid(p1+1,p3-p1-1);
					sSubWhere.replace(sFldKey,pCondition->sCmpValue);
					sql = sql.left(p1)+sSubWhere+sql.right(sql.length()-p3-1);
				}
			}
			p2 = sql.find(sFldKey);
		}
		iConSn++;
	}
	if(m_iType == 1)//mdbF
		pDbOper = m_pDataWindowMgr->m_pMdbOper;
	else if(m_iType == 2)//db
		pDbOper = m_pDataWindowMgr->m_pDbOper;
	if(pDbOper == NULL)
		return NULL;
	if(pRs == NULL)
	{
		pRs = &m_Recordset;
		if(!m_bInnerRsIsDirty && m_bRef && (SDateTime::getNowSoc()-m_tRsUpdateSOC) <= C_REF_TIMEOUT_S)
			return pRs;
		m_tRsUpdateSOC = SDateTime::getNowSoc();
	}
	if(pRs == &m_Recordset)
	{
		m_bInnerRsIsDirty = false;
		m_InnerRsLock.lock();
	}
	pRs->clear();
	//int ret = pDbOper->RetrieveRecordset(sql,*pRs);
	//进行数据集变换
	if(m_iTransformType == 3)
	{
		//cross transform
		SRecordset rs;
		int i,cnt = pDbOper->RetrieveRecordset(sql,rs);
		CSsp_Reference *pRef = m_pDataWindowMgr->SearchReference(m_sCrossRefName);
		SRecordset *pRefRs = pRef->GetRecordset();
		if(pRef == NULL || m_iCrossRefColumnNo < 0 || m_iCrossRefColumnNo >= rs.GetColumns()
			|| m_iCrossDataColumnNo <= 0 || m_iCrossDataColumnNo > rs.GetColumns()
			|| pRefRs == NULL || cnt == 0)
		{
			if(pRef)
			{
				LOGERROR("数据集%s中存在无效的交叉引用名称:%s",m_sName.data(),m_sCrossRefName.data());
			}
			else if(pRefRs == NULL)
			{
				LOGERROR("数据集%s中交叉引用:%s无有效的记录!",m_sName.data(),m_sCrossRefName.data());
			}
			else if(m_iCrossRefColumnNo < 0 || m_iCrossRefColumnNo >= rs.GetColumns())
			{
				LOGERROR("数据集%s中交叉列序号:%d无效!",m_sName.data(),m_iCrossRefColumnNo+1);
			}
			else if(cnt == 0)
			{
				//记录为空
			}
			else
			{
				LOGERROR("数据集%s中交叉数据列序号:%d无效!",m_sName.data(),m_iCrossDataColumnNo+1);
			}
			if(pRs == &m_Recordset)
				m_InnerRsLock.unlock();
			return pRs;
		}
		int rows = 1;
		int cols = rs.GetColumns()+pRefRs->GetRows()-2;
		int j;
		bool bDiff;
		SRecord *pRec;
		SRecord *pRecBased = rs.GetRecord(0);
		//处理动态列标题
		if(!m_bCrossColumnProcessed)
		{
			if(m_iCrossDataColumnNo > m_iCrossRefColumnNo)
			{
				m_Columns.remove(m_iCrossDataColumnNo);
				m_Columns.remove(m_iCrossRefColumnNo);
			}
			else
			{
				m_Columns.remove(m_iCrossRefColumnNo);
				m_Columns.remove(m_iCrossDataColumnNo);
			}
			m_bCrossColumnProcessed = true;
			m_iCrossRawColumnCnt = m_Columns.count();
			while(m_Columns.count() > m_iCrossRawColumnCnt)
				m_Columns.remove(m_iCrossRawColumnCnt);
			//插入动态列
			for(i=0;i<pRefRs->GetRows();i++)
			{
				CSsp_DwColumn *pNewCol = new CSsp_DwColumn();
				pNewCol->m_sName = pRefRs->GetValue(i,1);
				pNewCol->m_pMdbField = NULL;
				pNewCol->m_iType = 0;
				pNewCol->m_pRef = NULL;
				m_Columns.append(pNewCol);
			}
		}

		//计算新记录集的行数
		for(i=1;i<cnt;i++)
		{
			pRec = rs.GetRecord(i);
			bDiff = false;
			for(j=0;j<rs.GetColumns();j++)
			{
				if(j == m_iCrossRefColumnNo || j == m_iCrossDataColumnNo)
					continue;
				if(pRec->GetValue(j) != pRecBased->GetValue(j))
				{
					bDiff = true;
					break;
				}
			}
			if(bDiff)
			{
				pRecBased = pRec;
				rows++;
			}
		}

		pRs->SetSize(rows,cols);
		//设置列名称
		int col = 0;
		for(j=0;j<rs.GetColumns();j++)
		{
			if(j == m_iCrossRefColumnNo || j == m_iCrossDataColumnNo)
				continue;
			pRs->SetColumnName(col++,rs.GetColumnName(j));
		}
		for(j=0;j<pRefRs->GetRows();j++)
		{
			pRs->SetColumnName(col++,pRefRs->GetValue(j,1));
		}

		//设置记录集值内容
		col = 0;
		pRecBased = rs.GetRecord(0);
		SRecord *pNewRec = pRs->GetRecord(0);
		int new_row = 0;
		for(j=0;j<rs.GetColumns();j++)
		{
			if(j == m_iCrossRefColumnNo || j == m_iCrossDataColumnNo)
				continue;
			pNewRec->SetValue(col++,pRecBased->GetValue(j));
		}
		SString cross_id,cross_value;
		for(i=1;i<cnt;i++)
		{
			pRec = rs.GetRecord(i);
			bDiff = false;
			for(j=0;j<rs.GetColumns();j++)
			{
				if(j == m_iCrossRefColumnNo)
					cross_id = pRec->GetValue(j);
				else if(j == m_iCrossDataColumnNo)
					cross_value = pRec->GetValue(j);
				else if(pRec->GetValue(j) != pRecBased->GetValue(j))
					bDiff = true;
			}
			if(bDiff)
			{
				pRecBased = pRec;
				pNewRec = pRs->GetRecord(++new_row);

				//复制固有列
				col = 0;
				for(j=0;j<rs.GetColumns();j++)
				{
					if(j == m_iCrossRefColumnNo || j == m_iCrossDataColumnNo)
						continue;
					pNewRec->SetValue(col++,pRec->GetValue(j));
				}
			}
			//写动态列
			for(j=0;j<pRefRs->GetRows();j++)
			{
				if(cross_id == pRefRs->GetValue(j,0))
				{
					pNewRec->SetValue(col+j,cross_value);
					break;
				}
			}
		}
	}
	else if(m_iTransformType == 1 || m_iTransformType == 2)
	{
		//转置
		SRecordset rs;
		int i,j;
		pDbOper->RetrieveRecordset(sql,rs);
		int rows = rs.GetRows();
		int cols = rs.GetColumns();
		if(m_iTransformType == 2)//仅数据转置
		{
			pRs->SetSize(cols,rows);
			for(i=0;i<rows;i++)
			{
				for(j=0;j<cols;j++)
				{
					pRs->SetValue(j,i,rs.GetValue(i,j));
				}
			}
		}
		else
		{
			//带标题
			pRs->SetSize(cols,rows+1);
			CSsp_DwColumn *pDwCol;
			for(j=0;j<cols;j++)
			{
				pDwCol = this->GetColumn(j);
				if(pDwCol == NULL)
					pRs->SetValue(j,0,rs.GetColumnName(j));
				else
					pRs->SetValue(j,0,pDwCol->m_sName);
			}
			for(i=0;i<rows;i++)
			{
				for(j=0;j<cols;j++)
				{
					pRs->SetValue(j,i+1,rs.GetValue(i,j));
				}
			}
		}
	}
	else
	{
		pDbOper->RetrieveRecordset(sql,*pRs);
	}

	//自动变换列数据的表示形式
	if(m_bAutoColumnTransform)
	{
		unsigned long pos=0;
		CSsp_DwColumn *pCol = m_Columns.FetchFirst(pos);
		int r = 0;
		int c = 0;
		while(pCol)
		{
			if(pCol->m_sConv.length() > 0)
			{
				for(r=0;r<pRs->GetRows();r++)
					pRs->SetValue(r,c,pCol->GetConvDb2Sys(pRs->GetValue(r,c)));
			}
			c++;
			pCol = m_Columns.FetchNext(pos);
		}
	}
// 	if(ret >= 0)
// 	{
// 		unsigned long pos=0;
// 		CSsp_DwColumn *pCol = m_Columns.FetchFirst(pos);
// 		int c = 0;
// 		while(pCol)
// 		{
// 			pRs->SetColumnName(c++,pCol->m_sName);
// 			pCol = m_Columns.FetchNext(pos);
// 		}
// 	}
	if(pRs == &m_Recordset)
		m_InnerRsLock.unlock();
	return pRs;
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  取内部数据集，如果数据超时则重新加载
// 作    者:  邵凯田
// 创建时间:  2015-8-12 9:41
// 参数说明:  @iTimeoutSec表示超时秒数
//         :  @sWhere表示替换条件
// 返 回 值:  SRecordset*
//////////////////////////////////////////////////////////////////////////
SRecordset* CSsp_Dataset::GetInnerRecordset(int iTimeoutSec,SString sWhere)
{
	if(m_bInnerRsIsDirty || (m_iType == 2 && iTimeoutSec > 0 && abs((int)SDateTime::getNowSoc() - (int)m_tRsUpdateSOC) > iTimeoutSec))
		return GetRecordset(NULL,sWhere);
	return &m_Recordset;
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  针对内存数据库时的记录更新回调处理函数，包括记录更新、插入、删除和表截断
// 作    者:  邵凯田
// 创建时间:  2015-8-6 13:55
// 参数说明:  @cbParam为当前记录集的对象指针
//         :  @sTable为表名
//         :  @eType为触发类型
//         :  @iTrgRows为触发行数
//         :  @iRowSize为每行的记录长度
//         :  @pTrgData为触发数据内容，总有效长度为iTrgRows*iRowSize
// 返 回 值:  NULL
//////////////////////////////////////////////////////////////////////////
BYTE* CSsp_Dataset::OnMdbTrgCallback(void* cbParam, SString &sTable, eMdbTriggerType eType,int iTrgRows,int iRowSize,BYTE *pTrgData)
{
	CSsp_Dataset *pThis = (CSsp_Dataset *)cbParam;
	if(pThis == NULL || pThis->m_iType != 1 )
		return NULL;
// 	SDatabasePool<SDatabase>* pPool = MDB->GetDatabasePool();
// 	if(pPool == NULL)
// 		return NULL;
// 	SMdb *pMdb = (SMdb*)pPool->GetDatabase();
// 	if(pMdb == NULL)
// 		return NULL;
// 	CMdbClient *pMdbClient = pMdb->GetMdbClient();
// 	pPool->Release(pMdb);
	//暂不刷新内置数据集
	if(pThis->m_iTransformType != 0 || pThis->m_bWithAggregate || eType == MDB_TRG_INSERT || eType == MDB_TRG_DELETE || eType == MDB_TRG_TRUNCATE)
	{		
		pThis->SetInnerRsIsDirty();//pThis->GetRecordset(NULL,"");
	}
	else if(eType == MDB_TRG_UPDATE)
	{
#if 1//约定所有表的第一个字段为int型的唯一键，通过该字段进行记录检索，提高速度
		int i,j;
		unsigned long pos=0;
		CSsp_DwColumn *pCol;
		BYTE *pRow = pTrgData;
		unsigned int iPrimKey;
		int rowidx;
		SRecord *pR;
		//初始运行数据集，重新提取数据
		SRecordset *pRs = pThis->m_tRsUpdateSOC==0?pThis->GetInnerRecordset():(&pThis->m_Recordset);//pThis->GetInnerRecordset();
		if(pRs == NULL)
			return NULL;
		int oidbytes = pRs->GetOidBytes();
		if(oidbytes > (int)sizeof(iPrimKey))
		{
			LOGWARN("OID长度[%d]不正确!将进行截断!",oidbytes);
			oidbytes = (int)sizeof(iPrimKey);
		}
		pThis->m_InnerRsLock.lock();
		for(i=0;i<iTrgRows;i++)
		{
			memcpy(&iPrimKey,pRow,oidbytes);
			pR = pThis->SearchRecordByOid(NULL,iPrimKey,rowidx);
			if(pR == NULL)
			{
				pRow += iRowSize;
				continue;
			}
			j = 0;
			pCol = pThis->m_Columns.FetchFirst(pos);
			while(pCol)
			{
				if(pCol->m_pMdbField != NULL && !pCol->m_bPKey)
				{
					if(pThis->m_bAutoColumnTransform && pCol->m_sConv.length()>0)
						pR->SetValue(j,pCol->GetConvDb2Sys(pThis->m_pMdbTrgClient->GetValueStr(pRow,pCol->m_pMdbField)));
					else
						pR->SetValue(j,pThis->m_pMdbTrgClient->GetValueStr(pRow,pCol->m_pMdbField));
				}
				pCol = pThis->m_Columns.FetchNext(pos);
				j++;
			}
			pRow += iRowSize;
		}
		pThis->m_InnerRsLock.unlock();
#else
		int i,j;
		unsigned long pos=0;
		CSsp_DwColumn *pCol;
		BYTE *pRow = pTrgData;
		SString sPrimKey;
		SRecord *pR;
		for(i=0;i<iTrgRows;i++)
		{
			sPrimKey = pThis->GetPrimKeyByRawRecord(NULL,pRow);
			pR = pThis->SearchRecordByPrimKey(NULL,sPrimKey);
			if(pR == NULL)
			{
				pRow += iRowSize;
				continue;
			}
			j = 0;
			pThis->m_InnerRsLock.lock();
			pCol = pThis->m_Columns.FetchFirst(pos);
			while(pCol)
			{
				if(pCol->m_pMdbField != NULL && !pCol->m_bPKey)
					pR->SetValue(j,pThis->m_pMdbTrgClient->GetValueStr(pRow,pCol->m_pMdbField));
				pCol = pThis->m_Columns.FetchNext(pos);
				j++;
			}
			pThis->m_InnerRsLock.unlock();
			pRow += iRowSize;
		}
#endif
	}
	unsigned long pos=0;
	pThis->m_Callbacks.lock();
	stuTriggerParam *p = pThis->m_Callbacks.FetchFirst(pos);
	while(p)
	{
		p->pCallback(p->cbParam,pThis->m_pMdbTrgClient,pThis,eType,iTrgRows,iRowSize,pTrgData);
		p = pThis->m_Callbacks.FetchNext(pos);
	}
	pThis->m_Callbacks.unlock();
	return NULL;
}

stuDwCondition* CSsp_Dataset::SearchConditon(int con_sn)
{
	unsigned long pos=0;
	stuDwCondition* p = m_DwConditons.FetchFirst(pos);
	while(p)
	{
		if(p->iConditionSn == con_sn)
			return p;
		p = m_DwConditons.FetchNext(pos);
	}
	return NULL;
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  初始化内存库的数据集，注册触发回调并加载初始数据
// 作    者:  邵凯田
// 创建时间:  2015-8-6 14:12
// 参数说明:  void
// 返 回 值:  void
//////////////////////////////////////////////////////////////////////////
void CSsp_Dataset::InitMdb()
{
	if(m_iType != 1 )
		return ;
	SDatabasePool<SDatabase>* pPool = MDB->GetDatabasePool();
	if(pPool == NULL)
		return ;
	SMdb *pMdb = (SMdb*)pPool->GetDatabase();
	if(pMdb == NULL)
		return ;
	m_pMdbTrgClient = ((SMdb*)pPool->GetDatabaseByIdx(pPool->GetPoolSize()-1))->GetMdbClient();
	CMdbClient *pMdbClient = pMdb->GetMdbClient();
	pMdbClient->GetTableFields(m_sTableName.data(), m_MdbFields);
	CSsp_DwColumn *pCol;
	unsigned long pos=0,pos2=0;
	SString sColNames = m_sSql.toLower();
	sColNames.replace((char*)"\n",(char*)" ");
	sColNames.replace((char*)"\t",(char*)" ");
	sColNames.stripWhiteSpace();
	sColNames = sColNames.left(sColNames.find(" from "));
	if(sColNames.left(7) == "select ")
		sColNames = sColNames.mid(7);
	pCol = m_Columns.FetchFirst(pos);
	int idx=1;
	while(pCol)
	{
		pCol->m_sDbName = SString::GetIdAttribute(idx++,sColNames,",").toLower().stripWhiteSpace();
		stuTableField *pMdbFld = m_MdbFields.FetchFirst(pos2);
		while(pMdbFld)
		{
			if(pCol->m_sDbName == pMdbFld->name)
			{
				pCol->m_pMdbField = pMdbFld;
				break;
			}
			pMdbFld = m_MdbFields.FetchNext(pos2);
		}
		if(pCol->m_pMdbField == NULL)
		{
			LOGERROR("未找内存库记录集[%s]字段[%s]对应的内存表字段!",m_sName.data(),pCol->m_sName.data());
		}
		pCol = m_Columns.FetchNext(pos);
	}
	m_pMdbTrgClient->RegisterTriggerCallback(OnMdbTrgCallback,this,m_sTableName,MDB_TRGFLG_ALL);
	pPool->Release(pMdb);
// 	//刷新内置数据集
// 	GetRecordset(NULL,"");
	return ;
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  释放MDB资源，包括回调函数和数据集内容
// 作    者:  邵凯田
// 创建时间:  2015-8-6 14:33
// 参数说明:  void
// 返 回 值:  void
//////////////////////////////////////////////////////////////////////////
void CSsp_Dataset::ReleaseMdb()
{
	if(m_iType != 1 )
		return ;
// 	SDatabasePool<SDatabase>* pPool = MDB->GetDatabasePool();
// 	if(pPool == NULL)
// 		return ;
// 	SMdb *pMdb = (SMdb*)pPool->GetDatabase();
// 	if(pMdb == NULL)
// 		return ;
	if(m_pMdbTrgClient == NULL)
		return;
	//CMdbClient *pMdbClient = pMdb->GetMdbClient();
	m_pMdbTrgClient->RemoveTriggerCallback(OnMdbTrgCallback,this,m_sTableName,MDB_TRGFLG_ALL);
//	pPool->Release(pMdb);
	m_pMdbTrgClient = NULL;
	return ;
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  注册数据集变更触发回调函数
// 作    者:  邵凯田
// 创建时间:  2015-8-6 14:44
// 参数说明:  @pFun为回调函数
//         :  @cbParam为回调参数，通常为调用对象的this指针
// 返 回 值:  void
//////////////////////////////////////////////////////////////////////////
void CSsp_Dataset::RegisterTriggerCallback(OnMdbDatasetTrgCallback pFun,void *cbParam)
{
	unsigned long pos=0;
	m_Callbacks.lock();
	stuTriggerParam *p = m_Callbacks.FetchFirst(pos);
	while(p)
	{
		if(p->pCallback == pFun && p->cbParam == cbParam)
		{
			m_Callbacks.unlock();
			return;//exist it
		}
		p = m_Callbacks.FetchNext(pos);
	}
	m_Callbacks.unlock();

	stuTriggerParam *pParam = new stuTriggerParam();
	pParam->pCallback  = pFun;
	pParam->cbParam = cbParam;
	m_Callbacks.append(pParam);
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  删除数据集变更触发回调函数
// 作    者:  邵凯田
// 创建时间:  2015-8-6 14:44
// 参数说明:  @pFun为回调函数
//         :  @cbParam为回调参数，通常为调用对象的this指针
// 返 回 值:  void
//////////////////////////////////////////////////////////////////////////
void CSsp_Dataset::RemoveTriggerCallback(OnMdbDatasetTrgCallback pFun,void *cbParam)
{
	unsigned long pos=0;
	m_Callbacks.lock();
	stuTriggerParam *p = m_Callbacks.FetchFirst(pos);
	while(p)
	{
		if(p->pCallback == pFun && p->cbParam == cbParam)
		{
			m_Callbacks.remove(p,false);
			break;
		}
		p = m_Callbacks.FetchNext(pos);
	}
	m_Callbacks.unlock();
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  从指定的记录集查找对应原始数据行的的记录指针
// 作    者:  邵凯田
// 创建时间:  2015-8-6 15:18
// 参数说明:  @pRs为记录集指针，NULL表示内置记录集
//         :  @pRowData为原始行数据（即内存结构体），一般用于触发数据定位数据行
// 返 回 值:  SRecord*,NULL表示未找到
//////////////////////////////////////////////////////////////////////////
SRecord* CSsp_Dataset::SearchRecordByPrimKey(SRecordset *pRs,BYTE *pRowData)
{
	SString sPrimKeyValues;
	unsigned long pos=0;
	CSsp_DwColumn *pCol = m_Columns.FetchFirst(pos);
	while(pCol)
	{
		if(pCol->m_bPKey && pCol->m_pMdbField)
			sPrimKeyValues += CMdbClient::GetValueStr(pRowData,pCol->m_pMdbField);
		pCol = m_Columns.FetchNext(pos);
	}
	return SearchRecordByPrimKey(pRs,sPrimKeyValues);
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  取指针记录集原始行对应的主键字符串值，多主键用逗号分隔
// 作    者:  邵凯田
// 创建时间:  2015-8-7 15:11
// 参数说明:  @pRs为记录集指针，NULL表示内置记录集
//         :  @pRowData为原始行数据（即内存结构体），一般用于触发数据定位数据行
// 返 回 值:  SString
//////////////////////////////////////////////////////////////////////////
SString CSsp_Dataset::GetPrimKeyByRawRecord(SRecordset *pRs,BYTE *pRowData)
{
	SString sPrimKeyValues;
	unsigned long pos=0;
	CSsp_DwColumn *pCol = m_Columns.FetchFirst(pos);
	while(pCol)
	{
		if(pCol->m_bPKey && pCol->m_pMdbField)
			sPrimKeyValues += CMdbClient::GetValueStr(pRowData,pCol->m_pMdbField);
		pCol = m_Columns.FetchNext(pos);
	}
	return sPrimKeyValues;
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  从指定的记录集查找对应的主键值行的记录指针
// 作    者:  邵凯田
// 创建时间:  2015-8-6 16:33
// 参数说明:  @pRs为记录集指针，NULL表示内置记录集
//         :  @sPrimKeyValues为主键值字符中，多主键时该值为逗号分隔（结尾不加）
// 返 回 值:  SRecord*，NULL表示未找到
//////////////////////////////////////////////////////////////////////////
SRecord* CSsp_Dataset::SearchRecordByPrimKey(SRecordset *pRs,SString &sPrimKeyValues)
{
	if(m_PrimKeyColumns.count() == 0)
		return NULL;
	if(pRs == NULL)
	{
		pRs = &m_Recordset;		
	}
	int i,rows = pRs->GetRows();
// 	if(rows == 0)
// 	{
// 		GetRecordset(pRs,"");
// 		rows = pRs->GetRows();
// 	}
	if(pRs == &m_Recordset)
		m_InnerRsLock.lock();
	CSsp_DwColumn *pCol;
	if(m_PrimKeyColumns.count() == 1)
	{
		pCol = m_PrimKeyColumns[0];
		for(i=0;i<rows;i++)
		{
			SRecord *pR = pRs->GetRecord(i);
			if(pR->GetValue(pCol->m_iIdx) == sPrimKeyValues)
			{
				if(pRs == &m_Recordset)
					m_InnerRsLock.unlock();
				return pR;
			}
		}
		if(pRs == &m_Recordset)
			m_InnerRsLock.unlock();
		return NULL;
	}
	unsigned long pos=0;
	SString str;
	for(i=0;i<rows;i++)
	{
		str = "";
		SRecord *pR = pRs->GetRecord(i);
		pCol = m_PrimKeyColumns.FetchFirst(pos);
		while(pCol)
		{
			str += pR->GetValue(pCol->m_iIdx);
			pCol = m_PrimKeyColumns.FetchNext(pos);
			if(pCol != NULL)
				str += ",";
		}
		if(str == sPrimKeyValues)
		{
			if(pRs == &m_Recordset)
				m_InnerRsLock.unlock();
			return pR;
		}
	}
	if(pRs == &m_Recordset)
		m_InnerRsLock.unlock();
	return NULL;
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  从指定的记录集查找对应的主键行，需要在外部加锁
// 作    者:  邵凯田
// 创建时间:  2015-8-17 10:12
// 参数说明:  @pRs为记录集指针，NULL表示内置记录集
//         :  @oid表示唯一键值，对应表中第一列的int型字段
// 返 回 值:  SRecord*，NULL表示未找到
//////////////////////////////////////////////////////////////////////////
SRecord* CSsp_Dataset::SearchRecordByOid(SRecordset *pRs,unsigned int oid,int &rowidx)
{
	if(pRs == NULL)
	{
		pRs = &m_Recordset;		
	}
// 	if(pRs == &m_Recordset)
// 		m_InnerRsLock.lock();
	int i,rows = pRs->GetRows();
	SRecord *pR;
	for(i=0;i<rows;i++)
	{
		pR = pRs->GetRecord(i);
		if(pR->GetOid() == oid )
		{
// 			if(pRs == &m_Recordset)
// 				m_InnerRsLock.unlock();
			rowidx = i;
			return pR;
		}
	}

// 	if(pRs == &m_Recordset)
// 		m_InnerRsLock.unlock();
	return NULL;
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  设置当前数据集的动态查询条件
// 作    者:  邵凯田
// 创建时间:  2015-10-29 14:09
// 参数说明:  @condition为已生效条件的队列，未生效条件不能包含在队列中,该队列不应设置为自动释放，
//         :  调用后队列内容将被移动到类中，并由类负责释放
// 返 回 值:  void
//////////////////////////////////////////////////////////////////////////
void CSsp_Dataset::SetCondition(SPtrList<stuDwCondition> &condition)
{
	m_DwConditons.clear();
	condition.copyto(m_DwConditons);
	condition.clear();
}

//////////////////////////////////////////////////////////////////////////
// 名    称:  CSsp_Reference
// 作    者:  邵凯田
// 创建时间:  2015-7-14 9:43
// 描    述:  引用类
//////////////////////////////////////////////////////////////////////////
CSsp_Reference::CSsp_Reference()
{
	m_bRef = true;
}

CSsp_Reference::~CSsp_Reference()
{

}

//////////////////////////////////////////////////////////////////////////
// 描    述:  根据引用关键字返回引用值，如找不到对应关键字时则直接返回关键字
// 作    者:  邵凯田
// 创建时间:  2015-8-5 17:33
// 参数说明:  @sRefKey为引用关键字
// 返 回 值:  SString
//////////////////////////////////////////////////////////////////////////
SString CSsp_Reference::GetRefValue(SString sRefKey)
{
	SRecordset *pRs = GetRecordset(NULL,"");
	if(pRs == NULL)
		return sRefKey;
	int i,cnt = pRs->GetRows();
	for(i=0;i<cnt;i++)
	{
		if(pRs->GetValue(i,0) == sRefKey)
			return pRs->GetValue(i,1);
	}
	return sRefKey;
}


//////////////////////////////////////////////////////////////////////////
// 名    称:  CSsp_DatawindowMgr
// 作    者:  邵凯田
// 创建时间:  2015-7-14 9:43
// 描    述:  数据窗口管理类
//////////////////////////////////////////////////////////////////////////
CSsp_DatawindowMgr::CSsp_DatawindowMgr()
{
	SetConfigType(SPCFG_DATA_WINDOW);
	m_Datasets.setAutoDelete(true);
	m_Datasets.setShared(true);
	m_References.setAutoDelete(true);
	m_References.setShared(true);
	m_pMdbOper = NULL;
	m_pDbOper = NULL;
	m_iState = 0;
	//SKT_CREATE_THREAD(ThreadRefresh,this); //去除这句后，析构也要去除
}

CSsp_DatawindowMgr::~CSsp_DatawindowMgr()
{
	m_Datasets.clear();
	m_References.clear();
// 	m_iState = 1;
// 	while(m_iState == 1)
// 		SApi::UsSleep(10000);
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  从配置文件加载DATAWINDOW配置
// 作    者:  邵凯田
// 创建时间:  2015-7-14 10:13
// 参数说明:  @sFile为文件名
// 返 回 值:  true表示加载成功，false表示加载失败
//////////////////////////////////////////////////////////////////////////
bool CSsp_DatawindowMgr::Load(SString sFile)
{
	m_References.clear();
	m_Datasets.clear();
	CConfigBase::Load(sFile);
	SXmlConfig xml;
	if(!xml.ReadConfig(sFile))
	{
		LOGERROR("Load Datawindow config error!file=%s",sFile.data());
		return false;
	}
	return LoadCfg(&xml);
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  从配置类加载DataWindow配置
// 作    者:  邵凯田
// 创建时间:  2015-7-14 10:15
// 参数说明:  @pDW为数据窗口的配置实例指针
// 返 回 值:  true表示加载成功，false表示加载失败
//////////////////////////////////////////////////////////////////////////
bool CSsp_DatawindowMgr::LoadCfg(SBaseConfig *pDW)
{
	if(pDW->GetNodeName() != "data_window")
	{
		LOGERROR("Datawindow root node is not 'data_window'!");
		return false;
	}
	//先清除以前的数据
	m_Datasets.clear();
	m_References.clear();
	int i,cnt;
	cnt = pDW->GetChildCount("reference");
	for(i=0;i<cnt;i++)
	{
		SBaseConfig *pRefCfg = pDW->GetChildNode(i,"reference");
		CSsp_Reference *pRef = new CSsp_Reference();
		pRef->m_pDataWindowMgr = this;
		pRef->m_sName = pRefCfg->GetAttribute("name");
		pRef->m_iTransformType = pRefCfg->GetAttributeI("transform_type");
		pRef->m_sCrossRefName = pRefCfg->GetAttribute("cross_ref");
		pRef->m_iCrossRefColumnNo = pRefCfg->GetAttributeI("cross_column")-1;
		pRef->m_iCrossDataColumnNo = pRefCfg->GetAttributeI("cross_data_column")-1;

		SString sType = pRefCfg->GetAttribute("type");//fixed/mdb/db
		if(sType == "fixed")
			pRef->m_iType = 0;
		else if(sType == "mdb")
			pRef->m_iType= 1;
		else if(sType == "db")
			pRef->m_iType = 2;
		else
		{
			LOGWARN("Unknown reference[%s] type=%s!",pRef->m_sName.data(),sType.data());
			delete pRef;
			continue;
		}
		pRef->m_sDesc = pRefCfg->GetAttribute("desc");
		if(pRef->m_iType == 0)//fixed
		{
			CSsp_DwColumn *pCol1 = new CSsp_DwColumn();
			pCol1->m_iIdx = 0;
			pCol1->m_iType = 0;
			pCol1->m_iWidth = 80;
			pCol1->m_sName = "id";
			pCol1->m_bPKey = true;
			pCol1->m_bUniqueKey = true;
			pCol1->m_bIsNull = false;
			CSsp_DwColumn *pCol2 = new CSsp_DwColumn();
			pCol2->m_iIdx = 1;
			pCol2->m_iType = 0;
			pCol2->m_iWidth = 160;
			pCol2->m_sName = "name";
			pRef->m_Columns.append(pCol1);
			pRef->m_PrimKeyColumns.append(pCol1);
			pRef->m_Columns.append(pCol2);
			int r,rows = pRefCfg->GetChildCount("row");
			pRef->m_InnerRsLock.lock();
			pRef->m_Recordset.SetSize(rows,2);
			pRef->m_Recordset.SetColumnName(0,"id");
			pRef->m_Recordset.SetColumnName(1,"name");
			SBaseConfig *pR;
			for(r=0;r<rows;r++)
			{
				pR = pRefCfg->GetChildNode(r,"row");
				if(pR == NULL)
					continue;
				pRef->m_Recordset.SetValue(r,0,pR->GetAttribute("id"));
				pRef->m_Recordset.SetValue(r,1,pR->GetAttribute("name"));
			}
			pRef->m_InnerRsLock.unlock();
		}
		else//sql
		{
			pRef->m_sSql = pRefCfg->SearchNodeValue("select").stripWhiteSpace();
			if(pRef->m_sSql.length() == 0)
			{
				LOGWARN("None select define in reference:%s!",pRef->m_sName.data());
				delete pRef;
				continue;
			}
		}
		m_References.append(pRef);
	}

	cnt = pDW->GetChildCount("dataset");
	for(i=0;i<cnt;i++)
	{
		SBaseConfig *pDsCfg = pDW->GetChildNode(i,"dataset");
		CSsp_Dataset *pDs = new CSsp_Dataset();
		pDs->m_pDataWindowMgr = this;
		pDs->m_sName = pDsCfg->GetAttribute("name");
		pDs->m_iTransformType = pDsCfg->GetAttributeI("transform_type");
		pDs->m_sCrossRefName = pDsCfg->GetAttribute("cross_ref");
		pDs->m_iCrossRefColumnNo = pDsCfg->GetAttributeI("cross_column")-1;
		pDs->m_iCrossDataColumnNo = pDsCfg->GetAttributeI("cross_data_column")-1;
		pDs->m_sExtAttr = pDsCfg->GetAttribute("ext_attr");
		SString sType = pDsCfg->GetAttribute("type");//mdb/db
		if(sType == "mdb")
			pDs->m_iType= 1;
		else if(sType == "db")
			pDs->m_iType = 2;
		else
		{
			LOGWARN("Unknown dataset[%s] type=%s!",pDs->m_sName.data(),sType.data());
			delete pDs;
			continue;
		}
		pDs->m_sDesc = pDsCfg->GetAttribute("desc");
		pDs->m_sSql = pDsCfg->SearchNodeValue("select").stripWhiteSpace();
		if(pDs->m_sSql.length() == 0)
		{
			LOGWARN("None select define in dataset:%s!",pDs->m_sName.data());
			delete pDs;
			continue;
		}
		//加载列信息
		SBaseConfig *pColumn = pDsCfg->SearchChild("columns");
		if(pColumn != NULL)
		{
			int c,cols = pColumn->GetChildCount("col");
			SBaseConfig *pColCfg;
			for(c=0;c<cols;c++)
			{
				pColCfg = pColumn->GetChildNode(c,"col");
				CSsp_DwColumn *pCol = new CSsp_DwColumn(pColCfg);
				pCol->m_iIdx = c;
				if(pColCfg->GetAttribute("ref").length() > 0)
					pCol->m_pRef = SearchReference(pColCfg->GetAttribute("ref"));
				pDs->m_Columns.append(pCol);
				if(pCol->m_bPKey)
					pDs->m_PrimKeyColumns.append(pCol);
			}
		}
		if(pDs->m_iType == 1)
		{
			SString str = pDs->m_sSql.toLower();
			str = str.replace((char*)"\t",(char*)" ");
			str = str.replace((char*)"\n",(char*)" ");
			str = str.mid(str.find(" from ")+6);
			str = SString::GetIdAttribute(1,str," ");
			pDs->m_sTableName = str;
			pDs->InitMdb();
		}
		m_Datasets.append(pDs);
	}
	return true;
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  从数据库加载DataWindow配置
// 作    者:  邵凯田
// 创建时间:  2015-11-2 18:49
// 参数说明:  void
// 返 回 值:  true表示加载成功，false表示加载失败
//////////////////////////////////////////////////////////////////////////
bool CSsp_DatawindowMgr::LoadByDb()
{
	if(m_pDbOper == NULL)
		return false;
	SString sql;
	//先清除以前的数据
//不清除以前的数据，而是改为刷新
//	m_Datasets.clear();
//	m_References.clear();
	bool bNew;//是否为新的数据集
	unsigned long pos;
	int i,cnt,dw_sn;
	SRecordset rsDw;
	SRecord *pRec;
	sql = "select name,dbtype,dw_desc,dw_select,dw_sn,transform_type,cross_refname,cross_column,cross_data_column,ext_attr from t_ssp_data_window where dstype=1 order by cls_name desc,dw_desc";
	m_pDbOper->Retrieve(sql,rsDw);
	cnt = rsDw.GetRows();
	for(i=0;i<cnt;i++)
	{
		pRec = rsDw.GetRecord(i);
		//查找之前的引用
		CSsp_Reference *pRef = m_References.FetchFirst(pos);
		while(pRef)
		{
			if(pRef->GetName() == pRec->GetValue(0))
				break;
			pRef = m_References.FetchNext(pos);
		}
		if(pRef == NULL)
		{
			pRef = new CSsp_Reference();
			bNew = true;
		}
		else
			bNew = false;
		//CSsp_Reference *pRef = new CSsp_Reference();
		pRef->m_pDataWindowMgr = this;
		pRef->m_sName = pRec->GetValue(0);
		dw_sn = pRec->GetValue(4).toInt();
		pRef->m_iTransformType = pRec->GetValue(5).toInt();
		pRef->m_sCrossRefName = pRec->GetValue(6);
		pRef->m_iCrossRefColumnNo = pRec->GetValue(7).toInt()-1;
		pRef->m_iCrossDataColumnNo = pRec->GetValue(8).toInt()-1;
		pRef->m_sExtAttr = pRec->GetValue(9);
		//数据库类型：1-固定，2-历史库，3-内存库
		switch(pRec->GetValue(1).toInt())
		{
		case 1:
			pRef->m_iType = 0;
			break;
		case 2:
			pRef->m_iType = 2;
			break;
		case 3:
			pRef->m_iType = 1;
			break;
		default:
			{
				LOGWARN("Unknown reference[%s] type=%d!",pRef->m_sName.data(),pRec->GetValue(1).toInt());
				if(bNew)
					delete pRef;
				else
					m_References.remove(pRef);
				continue;
			}
		}
		
		pRef->m_sDesc = pRec->GetValue(2);
		if(pRef->m_iType == 0)//fixed
		{
			if(bNew)
			{
				CSsp_DwColumn *pCol1 = new CSsp_DwColumn();
				pCol1->m_iIdx = 0;
				pCol1->m_iType = 0;
				pCol1->m_iWidth = 80;
				pCol1->m_sName = "id";
				pCol1->m_bPKey = true;
				pCol1->m_bUniqueKey = true;
				pCol1->m_bIsNull = false;
				CSsp_DwColumn *pCol2 = new CSsp_DwColumn();
				pCol2->m_iIdx = 1;
				pCol2->m_iType = 0;
				pCol2->m_iWidth = 160;
				pCol2->m_sName = "name";
				pRef->m_Columns.append(pCol1);
				pRef->m_PrimKeyColumns.append(pCol1);
				pRef->m_Columns.append(pCol2);
			}
			pRef->m_InnerRsLock.lock();
			sql.sprintf("select row_id,row_name from t_ssp_data_window_row where dw_sn=%d order by row_sn",dw_sn);
			m_pDbOper->Retrieve(sql,pRef->m_Recordset);
			pRef->m_InnerRsLock.unlock();
		}
		else//sql
		{
			pRef->m_sSql = pRec->GetValue(3);
			if(pRef->m_sSql.length() == 0)
			{
				LOGWARN("None select define in reference:%s!",pRef->m_sName.data());
				if(bNew)
					delete pRef;
				else
					m_References.remove(pRef);
				continue;
			}
		}
		if(bNew)
			m_References.append(pRef);
	}

	sql = "select name,dbtype,dw_desc,dw_select,dw_sn,transform_type,cross_refname,cross_column,cross_data_column,ext_attr,cls_name from t_ssp_data_window where dstype=2 order by cls_name desc,dw_desc";
	m_pDbOper->Retrieve(sql,rsDw);
	cnt = rsDw.GetRows();
	for(i=0;i<cnt;i++)
	{
		pRec = rsDw.GetRecord(i);
		//查找之前的引用
		CSsp_Dataset *pDs = m_Datasets.FetchFirst(pos);
		while(pDs)
		{
			if(pDs->GetName() == pRec->GetValue(0))
				break;
			pDs = m_Datasets.FetchNext(pos);
		}
		if(pDs == NULL)
		{
			pDs = new CSsp_Dataset();
			bNew = true;
		}
		else
			bNew = false;

		//CSsp_Dataset *pDs = new CSsp_Dataset();
		pDs->m_pDataWindowMgr = this;
		pDs->m_sClsName = pRec->GetValue(10);
		pDs->m_sName = pRec->GetValue(0);
		dw_sn = pRec->GetValue(4).toInt();
		pDs->m_iTransformType = pRec->GetValue(5).toInt();
		pDs->m_sCrossRefName = pRec->GetValue(6);
		pDs->m_iCrossRefColumnNo = pRec->GetValue(7).toInt()-1;
		pDs->m_iCrossDataColumnNo = pRec->GetValue(8).toInt()-1;
		pDs->m_sExtAttr = pRec->GetValue(9);
		//数据库类型：1-固定，2-历史库，3-内存库
		switch(pRec->GetValue(1).toInt())
		{
		case 2:
			pDs->m_iType = 2;
			break;
		case 3:
			pDs->m_iType = 1;
			break;
		default:
			{
				LOGWARN("Unknown reference[%s] type=%d!",pDs->m_sName.data(),pRec->GetValue(1).toInt());
				if(bNew)
					delete pDs;
				else
					m_Datasets.remove(pDs);
				continue;
			}
		}
		pDs->m_sDesc = pRec->GetValue(2);
		pDs->m_sSql = pRec->GetValue(3);
		if(pDs->m_sSql.length() == 0)
		{
			LOGWARN("None select define in dataset:%s!",pDs->m_sName.data());
			if(bNew)
				delete pDs;
			else
				m_Datasets.remove(pDs);
			continue;
		}
		//加载列信息
		sql.sprintf("select col_name,col_vtype,col_width,ref_name,pkey,isnull,ukey,conv,ext_attr from t_ssp_data_window_col where dw_sn=%d order by col_sn",dw_sn);
		SRecordset rs;
		m_pDbOper->Retrieve(sql,rs);
		
		if(rs.GetRows() > 0)
		{
			pDs->m_PrimKeyColumns.clear();
			int c,cols = rs.GetRows();
			while(pDs->m_Columns.count()>cols)
				pDs->m_Columns.remove(cols);
			for(c=0;c<cols;c++)
			{
				pRec = rs.GetRecord(c);
				CSsp_DwColumn *pCol = pDs->GetColumn(c);
				if(pCol == NULL)
				{
					pCol = new CSsp_DwColumn();
					pDs->m_Columns.append(pCol);
				}
				//CSsp_DwColumn *pCol = new CSsp_DwColumn();
				pCol->m_pMdbField = NULL;
				if(pRec->GetValue(4).toInt() == 1)
					pCol->m_bPKey = true;
				else
					pCol->m_bPKey = false;
				if(pRec->GetValue(5).toInt() == 1)
					pCol->m_bIsNull = false;
				else
					pCol->m_bIsNull = true;
				if(pRec->GetValue(6).toInt() == 1)
					pCol->m_bUniqueKey = true;
				else
					pCol->m_bUniqueKey = false;
				if(pCol->m_bPKey == true)
				{
					pCol->m_bIsNull = false;//主键时必须为非空
					pCol->m_bUniqueKey = true;//主键必须为唯一键
				}
				if(pCol->m_bUniqueKey == true)
				{
					pCol->m_bIsNull = false;//主键时必须为非空
				}

				pCol->m_sName = pRec->GetValue(0);//列名称
				pCol->m_sConv = pRec->GetValue(7);//转换方法
				pCol->m_sExtAttr = pRec->GetValue(8);//扩展属性
				pCol->m_iWidth = pRec->GetValue(2).toInt();//列显示宽度
				SString sType = pRec->GetValue(1);//列数值类型，0:字符串;1:整数;2:浮点数;
				sType.toLower();
				if(sType == "int")
					pCol->m_iType = 1;
				else if(sType == "float")
					pCol->m_iType = 2;
				else
					pCol->m_iType = 0;//string
// 				int i,cnt = pColCfg->GetAttributeCount();
// 				for(i=0;i<cnt;i++)
// 				{
// 					sType = pColCfg->GetAttributeName(i);
// 					if(sType == "name" || sType == "width" || sType == "type" || sType == "ref")
// 						continue;
// 					pCol->m_sExtAttr += sType+"="+pColCfg->GetAttribute(sType)+";";
// 				}
				pCol->m_pRef = NULL;


				pCol->m_iIdx = c;
				if(pRec->GetValue(3).length() > 0)
					pCol->m_pRef = SearchReference(pRec->GetValue(3));
				if(pCol->m_bPKey)
					pDs->m_PrimKeyColumns.append(pCol);
			}
		}
		if(pDs->m_iType == 1)
		{
			SString str = pDs->m_sSql.toLower();
			str = str.replace((char*)"\t",(char*)" ");
			str = str.replace((char*)"\n",(char*)" ");
			str = str.mid(str.find(" from ")+6);
			str = SString::GetIdAttribute(1,str," ");
			pDs->m_sTableName = str;
			pDs->InitMdb();
		}
		m_Datasets.append(pDs);
	}
	return true;
}


//////////////////////////////////////////////////////////////////////////
// 描    述:  检索指定名称的引用实例指针
// 作    者:  邵凯田
// 创建时间:  2015-7-14 11:06
// 参数说明:  @sRefName为引用名称
// 返 回 值:  CSsp_Reference*，NULL表示未找到
//////////////////////////////////////////////////////////////////////////
CSsp_Reference* CSsp_DatawindowMgr::SearchReference(SString sRefName)
{
	unsigned long pos=0;
	CSsp_Reference *p = m_References.FetchFirst(pos);
	while(p)
	{
		if(p->m_sName.compareNoCase(sRefName) == 0)
			return p;
		p = m_References.FetchNext(pos);
	}
	return NULL;
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  检索指定名称的数据集实例指针
// 作    者:  邵凯田
// 创建时间:  2015-7-14 11:55
// 参数说明:  @sDsName为数据集名称
// 返 回 值:  CSsp_Dataset*，NULL表示未找到
//////////////////////////////////////////////////////////////////////////
CSsp_Dataset* CSsp_DatawindowMgr::SearchDataset(SString sDsName)
{
	unsigned long pos=0;
	CSsp_Dataset *p = m_Datasets.FetchFirst(pos);
	while(p)
	{
		if(p->m_sName == sDsName)
			return p;
		p = m_Datasets.FetchNext(pos);
	}
	return NULL;
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  清除所有内容
// 作    者:  邵凯田
// 创建时间:  2015-8-17 11:21
// 参数说明:  void
// 返 回 值:  void
//////////////////////////////////////////////////////////////////////////
void CSsp_DatawindowMgr::Clear()
{
	unsigned long pos=0;
	CSsp_Dataset *p = m_Datasets.FetchFirst(pos);
	while(p)
	{
		p->ClearTriggerCallback();
		p->ReleaseMdb();
		p = m_Datasets.FetchNext(pos);
	}
	CSsp_Reference *p2 = m_References.FetchFirst(pos);
	while(p2)
	{
		p2->ClearTriggerCallback();
		p2->ReleaseMdb();
		p2 = m_References.FetchNext(pos);
	}
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  脏数据集自动刷新线程
// 作    者:  邵凯田
// 创建时间:  2015-12-21 16:06
// 参数说明:  this
// 返 回 值:  NULL
//////////////////////////////////////////////////////////////////////////
void* CSsp_DatawindowMgr::ThreadRefresh(void *lp)
{
	CSsp_DatawindowMgr *pThis = (CSsp_DatawindowMgr*)lp;
	unsigned long pos=0;
	CSsp_Dataset *p;
	g_ssp_datasets.setShared(true);

	while(pThis->m_iState == 0)
	{
		g_ssp_datasets.lock();
		p = g_ssp_datasets.FetchFirst(pos);
		while(p)
		{
			if(p->IsInnerRsIsDirty())
				p->GetInnerRecordset(0,"");
			p = g_ssp_datasets.FetchNext(pos);
		}
		g_ssp_datasets.unlock();
		SApi::UsSleep(10000);
	}
	pThis->m_iState = 2;
	return NULL;
}

