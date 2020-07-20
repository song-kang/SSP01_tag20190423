/**
 *
 * 文 件 名 : dbInteraction.h
 * 创建日期 : 2015-7-30 15:50
 * 作    者 : 褚冰冰
 * 修改日期 : 2015-7-30 15:50
 * 当前版本 : 1.0
 * 功能描述 : 添加特定数据库操作功能
 * 修改记录 : 
 *            $Log: $
 *
 * Ver  Date        Author  Comments
 * ---  ----------  ------  -------------------------------------------
 * 001	2015-7-30 	褚冰冰　添加特定数据库操作功能
 *
 **/
#ifndef _DBINTERACTION_H
#define _DBINTERACTION_H
#include <QTreeWidgetItem>
#include <QMap>
#include "db/SDatabase.h"
#include "include_macdef.h"
#include "structDefine.h"
#include "pcellTree.h"
#include "mcombobox.h"
typedef int SQLTYPE;
class dbInteraction
{
public:
	dbInteraction(SDatabaseOper * pDb);
	dbInteraction()
	{
		m_db = NULL;
	}
	~dbInteraction(void);
	void setDb(SDatabaseOper* pDb){m_db = pDb;};
	SDatabaseOper* getDb(){return m_db;}
	//获取树结构表中的所有下级节点
	//参数说明：sTable   表名
	//		    sField   主键字段名
	//          sFField  父字段名
	//          sValue   要列出的节点、对应sField
	//          sSelect  要查找的属性
	//
	virtual void addData(SRecordset &rs,SString sData,QTreeWidgetItem *item){S_UNUSED(rs);S_UNUSED(sData);S_UNUSED(item);};
	void GetChildren(SString sTable,SString sField,SString sFField,SString sValue,QTreeWidgetItem*&item);

	//将树类型的数据表中内容存储到树结构中
	//参数说明：sTable   表名
	//		    sField   主键字段名
	//          sFField  父字段名
	//          sValue   要列出的节点、对应sField
	//          sSelect  要查找的属性
	//
	template<class T> void getTreeItem(SString sTable,SString sField,SString sFField,SString sValue,QTreeWidgetItem*&item,T &t_list)
	{
		SRecordset rs;
		SString Ssql = SString::toFormat("select * from %s",sTable.data());
		m_db->Retrieve(Ssql,rs);
		for (int i=0;i<rs.GetColumns();i++)
		{
			if (rs.GetColumnName(i)!=sFField)
			{
				t_list.push_back(rs.GetColumnName(i).data());
			}
		}
		GetChildren(sTable,sField,sFField,sValue,item);
		//return m_tree;
	}
	mCombobox * getRefNum();//获取引用序号引用
	mCombobox * getReportNum();//获取报表引用
	void setRefIndex(mCombobox *pComb,QString val);//根据值设置引用序号引用的索引
	void initUicfgAndReportRef();
public:
	SDatabaseOper* m_db;
	SRecordset m_recored;
	QMap<QString,QString> m_uicfgWndMap;//t_ssp_uicfg_wnd
	QMap<QString,QString> m_reportInfoMap;//t_ssp_report_info
};
#endif