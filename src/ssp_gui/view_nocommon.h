/**
 *
 * 文 件 名 : view_nocommon.h
 * 创建日期 : 2015-7-30 15:35
 * 作    者 : 褚冰冰
 * 修改日期 : 2015-7-30 15:35
 * 当前版本 : 1.0
 * 功能描述 : 特定窗口实现
 * 修改记录 : 
 *            $Log: $
 *
 * Ver  Date        Author  Comments
 * ---  ----------  ------  -------------------------------------------
 * 001	2015-7-30 	褚冰冰　特定窗口实现
 *
 **/

#ifndef VIEW_NOCOMMON_H
#define VIEW_NOCOMMON_H

#include <QWidget>
#include "ui_view_nocommon.h"
#include "ssp_baseview.h"
#include "db/mysql/SMySQL.h"
#include "dbInteraction.h"
#include "include_macdef.h"
#include "structDefine.h"
#include "ssp_datawindow.h"
#include "mtreewidget.h"
#include "mcombobox.h"
#include "record_sql.h"
#include "clearData.h"

class ViewNoCommonPlugin;
class view_nocommon : public CBaseView,public clearData
{
	Q_OBJECT

public:
	view_nocommon(QWidget *parent = 0,SString sFunName="");
	~view_nocommon();
	bool InitWidget(SString sWname);//初始化不同的非通用窗口
	void InitDb();//数据库初始化
	SString PrepareForFunPoint(SString sTop);//准备功能点数据表(空创建顶层)，并且返回顶层节点
	void SetAsFunPoint();//初始化为系统功能点
	void SetAsGrtUsr();//初始化为用户权限
	void SetAsGrtGrp();//初始化为群组权限
	virtual void OnPreShow();
	virtual void OnPreHide();
	void GetSysFunName(QVector<QString> &funList);

protected:
	virtual void dragEnterEvent(QDragEnterEvent *event);
	virtual void dropEvent(QDropEvent *event);

public slots:
	void OnAddNode(QTreeWidgetItem * node,QTreeWidgetItem *parentNode);
	void OnDelNode(QTreeWidgetItem * node,QTreeWidgetItem *parentNode);
	void OnEditNode(QTreeWidgetItem * node,QTreeWidgetItem *parentNode);
	void OnUpdateNode(QTreeWidgetItem * node,SString sPre,int column);

	void OnListItemChoosed_group(QListWidgetItem *item);
	void OnUpdateCheck_group(QTreeWidgetItem* item,int column);

	void OnListItemChoosed_user(QListWidgetItem *item);
	void OnUpdateCheck_user(QTreeWidgetItem* item,int column);
private:
	Ui::view_nocommon ui;
	SString m_funName;//功能名字
	SString m_tableName;//表名
	SDatabaseOper *m_pOper;//数据库操作对象
	pcell_CTree<SString> *m_tree;
	SRecordset* m_Record;
	record_sql m_sqlRecord;//命令集
	dbInteraction m_dbInter;
	QListWidgetItem *m_choosedItem;//当前列表选中项
	bool m_bconnect;
	SString m_funTop;//顶层功能点
	vector<SString> m_funFields;//功能点表字段
	ViewNoCommonPlugin *m_viewNoCommonPlugin;
};

#endif // VIEW_NOCOMMON_H
