/**
 *
 * 文 件 名 : record_sql.h
 * 创建日期 : 2015-7-30 15:29
 * 作    者 : 褚冰冰
 * 修改日期 : 2015-7-30 15:29
 * 当前版本 : 1.0
 * 功能描述 : 执行语句集
 * 修改记录 : 
 *            $Log: $
 *
 * Ver  Date        Author  Comments
 * ---  ----------  ------  -------------------------------------------
 * 001	2015-7-30 	褚冰冰　执行语句集
 *
 **/

#ifndef _RECORD_SQL_H
#define _RECORD_SQL_H

#include <vector>
#include "SLog.h"
#include "SList.h"
#include "SString.h"
#include "SApi.h"
#include "./db/SDatabase.h"
using namespace std;

//////////////////////////////////////////////////////////////////////////
// 名    称:  record_sql
// 作    者:  褚冰冰
// 创建时间:  2015-7-30 15:30
// 描    述:  sql语句记录集
//////////////////////////////////////////////////////////////////////////
class record_sql
{
public:
	typedef int SQLTYPE;//语句类型 0：insert 1:delete 2:update
public:
	record_sql(void);
	//////////////////////////////////////////////////////////////////////////
	// 描    述:  数据集是否为空
	// 作    者:  褚冰冰
	// 创建时间:  2015-7-30 15:32
	// 参数说明:  @
	// 返 回 值:  true:空 false:非空
	//////////////////////////////////////////////////////////////////////////
	bool isEmpty(){return m_insertList.empty()&&m_deltList.empty()&&m_updateList.empty();}

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  按顺序执行数据集中语句
	// 作    者:  褚冰冰
	// 创建时间:  2015-7-30 15:33
	// 参数说明:  @database 数据库操作指针
	// 返 回 值:  
	//////////////////////////////////////////////////////////////////////////
	bool exec(/*SDatabase * database*/);

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  向数据集添加语句
	// 作    者:  褚冰冰
	// 创建时间:  2015-7-30 15:34
	// 参数说明:  @sSql 要添加的语句
	//			  @type 语句类型
	// 返 回 值:  
	//////////////////////////////////////////////////////////////////////////
	void addSql(SString sSql,SQLTYPE type);
	void clear();
	~record_sql(void);
private:
	//SMySQL *m_pMysqlDb;
	vector<SString> m_insertList;//插入语句集
	vector<SString> m_deltList;//删除语句集
	vector<SString> m_updateList;//修改语句集
};

#endif