/**
*
* 文 件 名 : SspDbRestore.h
* 创建日期 : 2016-04-23 17:07
* 作    者 : SspAssist(skt001@163.com)
* 修改日期 : $Date: 2016-04-23 17:07$
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

#ifndef __SAXEX_CSSPDBRESTORE_H__
#define __SAXEX_CSSPDBRESTORE_H__

#include "SSaxXml.h"
#include "SDatabase.h"
#include "ssp_gui.h"
#include "SQt.h"

class CSspDbRestore : public SSaxXml
{
public:
	enum eNodeLevel
	{
		LEV_NONE=1,
		LEV_ROOT,
		LEV_TABLE,
		LEV_TABLE_COL,
		LEV_TABLE_ROW,
		LEV_TABLE_ROW_BLOB,
	};

	CSspDbRestore();
	virtual ~CSspDbRestore();

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  开始新的XML文档解析
	// 作    者:  邵凯田
	// 创建时间:  2016-04-23 17:07
	// 参数说明:  void
	// 返 回 值:  void
	//////////////////////////////////////////////////////////////////////////
	virtual void StartDocument();

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  结束XML文件的解析
	// 作    者:  邵凯田
	// 创建时间:  2016-04-23 17:07
	// 参数说明:  void
	// 返 回 值:  void
	//////////////////////////////////////////////////////////////////////////
	virtual void EndDocument();

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  开始一个新的XML节点元素
	// 作    者:  邵凯田
	// 创建时间:  2016-04-23 17:07
	// 参数说明:  @sNodeName表示节点名称
	//         :  @pAttribute表示节点属性指针，可含0~N个属性
	//         :  @iAttributeCount表示节点属性数量
	// 返 回 值:  void
	//////////////////////////////////////////////////////////////////////////
	virtual void StartElement(char* sNodeName,SSaxAttribute *pAttribute,int iAttributeCount);

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  结束一个XML节点元素
	// 作    者:  邵凯田
	// 创建时间:  2016-04-23 17:07
	// 参数说明:  @sNodeName表示节点名称
	// 返 回 值:  void
	//////////////////////////////////////////////////////////////////////////
	virtual void EndElement(char* sNodeName);

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  错误事件
	// 作    者:  邵凯田
	// 创建时间:  2016-04-23 17:07
	// 参数说明:  @iErrCode表示错误代码
	//         :  @sErrText表示错误描述
	// 返 回 值:  void
	//////////////////////////////////////////////////////////////////////////
	virtual void Error(int iErrCode,SString sErrText);

	virtual void ProcessEndSql()=0;

	SDatabase *m_pDb;
	eNodeLevel m_NodeLevel;
	SString m_sTableName;
	SString m_sql;
	SString m_sBlobColName;
	SString m_sBlobWhere;
	int m_iBlobSize;
	BYTE *m_pBlobBuffer;
	int m_iBlobBufferLen;
	bool m_bTrunkTab;//是否还原前清除数据

	int m_iSaveTabs;
	int m_iSaveRows;

	int m_iDbErrors;
	SString m_sTempVal;
};

class CSspDbRestoreMysql : public CSspDbRestore
{
public:
	CSspDbRestoreMysql(){};
	virtual ~CSspDbRestoreMysql(){};
	virtual void Characters(char* sNodeValue);
	virtual void ProcessEndSql();
};

class CSspDbRestoreOracle : public CSspDbRestore
{
public:
	CSspDbRestoreOracle(){};
	virtual ~CSspDbRestoreOracle(){};
	virtual void Characters(char* sNodeValue);
	virtual void ProcessEndSql();
};


#endif
