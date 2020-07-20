/**
*
* 文 件 名 : SspDbRestore.cpp
* 创建日期 : 2016-04-23 17:07
* 作    者 : SspAssist(skt001@163.com)
* 修改日期 : $Date: 2016-04-23$
* 当前版本 : $Revision: 1.0.0$
* 功能描述 : 基于SSaxXmlEx类的XML解释功能封装
* 修改记录 : 
*            $Log: $
*
* Ver  Date        Author  Comments
* ---  ----------  ------  -------------------------------------------
* 001  2016-04-23        　创建文件
*
**/

#include "SspDbRestore.h"

CSspDbRestore::CSspDbRestore()
{
	m_iBlobBufferLen = 0;
	m_pBlobBuffer = NULL;
}

CSspDbRestore::~CSspDbRestore()
{
	if(m_pBlobBuffer != NULL)
		delete[] m_pBlobBuffer;
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  开始新的XML文档解析
// 作    者:  邵凯田
// 创建时间:  2016-04-23 17:07
// 参数说明:  void
// 返 回 值:  void
//////////////////////////////////////////////////////////////////////////
void CSspDbRestore::StartDocument()
{
	m_iSaveTabs = m_iSaveRows = m_iDbErrors = 0;
	m_NodeLevel = LEV_NONE;
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  结束XML文件的解析
// 作    者:  邵凯田
// 创建时间:  2016-04-23 17:07
// 参数说明:  void
// 返 回 值:  void
//////////////////////////////////////////////////////////////////////////
void CSspDbRestore::EndDocument()
{
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  开始一个新的XML节点元素
// 作    者:  邵凯田
// 创建时间:  2016-04-23 17:07
// 参数说明:  @sNodeName表示节点名称
//         :  @pAttribute表示节点属性指针，可含0~N个属性
//         :  @iAttributeCount表示节点属性数量
// 返 回 值:  void
//////////////////////////////////////////////////////////////////////////
void CSspDbRestore::StartElement(char* sNodeName,SSaxAttribute *pAttribute,int iAttributeCount)
{
	switch(m_NodeLevel)
	{
	case LEV_TABLE_ROW:
		if(SString::equals(sNodeName,"blob"))
		{
			m_NodeLevel = LEV_TABLE_ROW_BLOB;
			for(int i=0;i<iAttributeCount;i++)
			{
				register char *pStr = pAttribute[i].m_sAttrName.data();
				if(SString::equals(pStr,"name"))
					m_sBlobColName = pAttribute[i].m_sAttrValue;
				else if(SString::equals(pStr,"size"))
					m_iBlobSize = pAttribute[i].m_sAttrValue.toInt();
				else if(SString::equals(pStr,"where"))
					m_sBlobWhere = pAttribute[i].m_sAttrValue;
			}
		}
		break;
	case LEV_TABLE:
		if(SString::equals(sNodeName,"r"))
			m_NodeLevel = LEV_TABLE_ROW;
		else if(SString::equals(sNodeName,"column"))
			m_NodeLevel = LEV_TABLE_COL;
		break;
	case LEV_ROOT:
		if(SString::equals(sNodeName,"table"))
		{
			m_NodeLevel = LEV_TABLE;
			m_sTableName = "";
			for(int i=0;i<iAttributeCount;i++)
			{
				register char *pStr = pAttribute[i].m_sAttrName.data();
				if(SString::equals(pStr,"name"))
				{
					m_sTableName = pAttribute[i].m_sAttrValue;
					break;
				}
			}
			ProcessEndSql();
			if(m_bTrunkTab)
			{
				if(!m_pDb->Execute(SString::toFormat("truncate table %s",m_sTableName.data())))
					m_iDbErrors++;
			}
		}
		break;
	case LEV_NONE:
		if(SString::equals(sNodeName,"ssp_db"))
			m_NodeLevel = LEV_ROOT;
		break;
	}

}

//////////////////////////////////////////////////////////////////////////
// 描    述:  结束一个XML节点元素
// 作    者:  邵凯田
// 创建时间:  2016-04-23 17:07
// 参数说明:  @sNodeName表示节点名称
// 返 回 值:  void
//////////////////////////////////////////////////////////////////////////
void CSspDbRestore::EndElement(char* sNodeName)
{
	switch(m_NodeLevel)
	{
	case LEV_TABLE_ROW:
		m_NodeLevel = LEV_TABLE;
		m_iSaveRows ++;
		if(m_iSaveRows % 100 == 0)
		{
			SSP_GUI->SetStatusTip(SString::toFormat("已导入%d张表,共%d条记录...",m_iSaveTabs,m_iSaveRows));
			SQt::ProcessUiEvents();
		}
		break;
	case LEV_TABLE_COL:
		m_NodeLevel = LEV_TABLE;
		break;
	case LEV_TABLE_ROW_BLOB:
		m_NodeLevel = LEV_TABLE_ROW;
		break;
	case LEV_TABLE:
		m_NodeLevel = LEV_ROOT;
		m_iSaveTabs ++;
		SSP_GUI->SetStatusTip(SString::toFormat("已导入%d张表,共%d条记录...",m_iSaveTabs,m_iSaveRows));
		ProcessEndSql();
		SQt::ProcessUiEvents();
		break;
	case LEV_ROOT:
		m_NodeLevel = LEV_NONE;
		break;
	}

}

//////////////////////////////////////////////////////////////////////////
// 描    述:  错误事件
// 作    者:  邵凯田
// 创建时间:  2016-04-23 17:07
// 参数说明:  @iErrCode表示错误代码
//         :  @sErrText表示错误描述
// 返 回 值:  void
//////////////////////////////////////////////////////////////////////////
void CSspDbRestore::Error(int iErrCode,SString sErrText)
{
	printf("error in saxxml!\n");
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  新的节点值内容
// 作    者:  邵凯田
// 创建时间:  2016-04-23 17:07
// 参数说明:  @sNodeValue表示指定的字符串首地址
// 返 回 值:  void
//////////////////////////////////////////////////////////////////////////
void CSspDbRestoreMysql::Characters(char* sNodeValue)
{
	//INSERT INTO t_ia_base_signal VALUES (3,'重合闸动作',1,'重合闸.*动作',1,NULL,NULL,NULL),(4,'断路器位置',1,'(断路器)|(开关)合位',NULL,NULL,NULL,NULL)
	if(m_NodeLevel == LEV_TABLE_ROW)
	{
		register char *psVal = sNodeValue;
		if(m_sql.length() == 0)
			m_sql.sprintf("insert into %s values (%s)",m_sTableName.data(),psVal);
		else
			m_sql += SString::toFormat(",(%s)",psVal);
		if(m_sql.length() > 65536)
		{
			if(!m_pDb->Execute(m_sql))
			{
				m_iDbErrors ++;
			}
			m_sql = "";
		}
	}
	else if(m_NodeLevel == LEV_TABLE_ROW_BLOB)
	{
		if(m_iBlobSize>m_iBlobBufferLen)
		{
			if(m_pBlobBuffer != NULL)
				delete[] m_pBlobBuffer;
			m_iBlobBufferLen = m_iBlobSize;
			m_pBlobBuffer = new BYTE[m_iBlobBufferLen];
		}
		SString::StrToHex(sNodeValue,strlen(sNodeValue),m_pBlobBuffer);
		ProcessEndSql();
		if(!m_pDb->UpdateLobFromMem(m_sTableName,m_sBlobColName,m_sBlobWhere,m_pBlobBuffer,m_iBlobSize))
			m_iDbErrors ++;
	}
}

void CSspDbRestoreMysql::ProcessEndSql()
{
	if(m_sql.length() == 0)
		return;
	if(!m_pDb->Execute(m_sql))
		m_iDbErrors ++;
	m_sql = "";
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  新的节点值内容
// 作    者:  邵凯田
// 创建时间:  2016-04-23 17:07
// 参数说明:  @sNodeValue表示指定的字符串首地址
// 返 回 值:  void
//////////////////////////////////////////////////////////////////////////
void CSspDbRestoreOracle::Characters(char* sNodeValue)
{
	//insert all into t_oe_run_unit values(4,'a','111') into t_oe_run_unit values(5,'b','222') into t_oe_run_unit values(6,'c','333') select 1 from dual
	if(m_NodeLevel == LEV_TABLE_ROW)
	{
		register char *psVal = sNodeValue;
//		m_sql.sprintf("insert into %s values (%s)",m_sTableName.data(),psVal);
		if(m_sql.length() == 0)
			m_sql.sprintf("insert all into %s values (%s)",m_sTableName.data(),psVal);
		else
			m_sql += SString::toFormat(" into %s values (%s)",m_sTableName.data(),psVal);

		if(m_sql.length() > 10240)
		{
			m_sql += " select 1 from dual";
			if(!m_pDb->Execute(m_sql))
				m_iDbErrors ++;
			m_sql = "";
		}
	}
	else if(m_NodeLevel == LEV_TABLE_ROW_BLOB)
	{
		if(m_iBlobSize>m_iBlobBufferLen)
		{
			if(m_pBlobBuffer != NULL)
				delete[] m_pBlobBuffer;
			m_iBlobBufferLen = m_iBlobSize;
			m_pBlobBuffer = new BYTE[m_iBlobBufferLen];
		}
		SString::StrToHex(sNodeValue,strlen(sNodeValue),m_pBlobBuffer);
		ProcessEndSql();
		if(!m_pDb->UpdateLobFromMem(m_sTableName,m_sBlobColName,m_sBlobWhere,m_pBlobBuffer,m_iBlobSize))
			m_iDbErrors ++;
	}
}

void CSspDbRestoreOracle::ProcessEndSql()
{
	if(m_sql.length() == 0)
		return;
	m_sql += " select 1 from dual";
	if(!m_pDb->Execute(m_sql))
		m_iDbErrors ++;
	m_sql = "";
}
