/**
 *
 * 文 件 名 : mtreewidget.h
 * 创建日期 : 2015-7-30 15:19
 * 作    者 : 褚冰冰
 * 修改日期 : 2015-7-30 15:19
 * 当前版本 : 1.0
 * 功能描述 : 重写QTreeWidget，添加自定义功能
 * 修改记录 : 
 *            $Log: $
 *
 * Ver  Date        Author  Comments
 * ---  ----------  ------  -------------------------------------------
 * 001	2015-7-30 	褚冰冰　重写QTreeWidget
 *
 **/

#ifndef MTREEWIDGET_H
#define MTREEWIDGET_H
#include "pcellTree.h"
#include <QTreeWidget>
#include <QMenu>
#include <QLineEdit>
#include <QMessageBox>
#include <QResizeEvent>
#include "SString.h"
#include "mcombobox.h"
#include "dbInteraction.h"
class mTreeWidget : public QTreeWidget,public dbInteraction
{
	Q_OBJECT

public:
	mTreeWidget(QWidget *parent=0);
	virtual ~mTreeWidget();
	void setManual(bool bManual)
	{
		m_bManual = bManual;
	}

	//将树结构数据添加到treewidget中
	template<class T> void addTree(SString sTable,SString sField,SString sFField,SString sValue,QStringList &ql,T &t_list)
	{
		if (m_bCheck)
		{
			this->setColumnCount(2);
		}else
		{
/*		int i = SString::GetAttributeCount(cTree->getData(),",");
		this->setColumnCount(i);*/	
			this->setHeaderLabels(ql);
		}
		QTreeWidgetItem * citm = new QTreeWidgetItem(this);
		dbInteraction::setDb(DB);
		initUicfgAndReportRef();
		dbInteraction::getTreeItem(sTable,sField,sFField,sValue,citm,t_list);
		this->resizeColumnToContents(0);
}

signals:
	void addNode(QTreeWidgetItem * node,QTreeWidgetItem *parentNode);
	void delNode(QTreeWidgetItem * node,QTreeWidgetItem *parentNode);
	void editNode(QTreeWidgetItem * node,QTreeWidgetItem *parentNode);
	void updateNode(QTreeWidgetItem * node,SString sPre,int column);//sPre为修改之前的主键值，在修改主键的时候用到
	void updateCheck(QTreeWidgetItem * node,int column);
public slots:
	void OnCurrentItemChanged ( QTreeWidgetItem * current, QTreeWidgetItem * previous );
	void onItemChanged(QTreeWidgetItem * item, int column);
	QTreeWidgetItem * chooseItem(int topIndex,QString sText);//根据名字找到对应选项
	QTreeWidgetItem * chooseItem(QTreeWidgetItem *topItem,QString sText);
	void addData(SRecordset &rs,SString sData,QTreeWidgetItem *item);//添加列数据
	void onCustomContextMenuRequested(const QPoint &pos);//右键弹起菜单
	void OnItemDoubleClicked(QTreeWidgetItem * item, int column);//双击事件
	void setRmenuAndCheck(bool b_rmenu,bool b_check,bool b_edit)//设置是否有右键菜单和勾选
	{
		m_bRmenu = b_rmenu;
		m_bCheck = b_check;
		m_bEdit = b_edit;
	}
	void emptyCheck(int topIndex);
	void emptyCheck(QTreeWidgetItem * item);//清空勾选
	void OnAddNode();
	void OnDelNode();
	void OnEditNode();
	void finishEdit();
	void oneCellChanged(QTreeWidgetItem* item,int column);//下拉框发生改变
	void setColumnToCheck(QTreeWidgetItem * item,int column);//将列设成勾选
	void setColumnToRef(QTreeWidgetItem * item,int column);//将列设成combobox
	void setColumnToUicfgOrReportRef(QTreeWidgetItem * item,int column);//根据T_SSP_UICFG_WND/T_SSP_REPORT_INFO装换成wnd_sn+name
private slots:
	void slotupLoadRowClicked();
	void slotDownLoadRowClicked();
private:
	int getLevel(QTreeWidgetItem *item);//获取节点是第几层
	bool eventFilter(QObject *obj, QEvent *event);
private:
	QTreeWidgetItem * m_choosed;
	QMenu * m_rcMenu;
	QAction * m_addNode;
	QAction * m_delNode;
	QAction * m_editNode;
	QTreeWidgetItem * m_citem;
	QTreeWidgetItem * m_pitem;
	bool m_bRmenu;//是否添加右键菜单
	bool m_bCheck;//是否可以勾选
	bool m_bEdit;//是否可以修改
	bool m_addOrUpdate;
	int m_clickCloumn;//当前双击的列
	bool m_bManual;//是否是手动勾选
	bool m_bresized;
	mCombobox *mpcomb;
	SDatabaseOper *m_dbOper;
	QWidget * widget;
};

#endif // MTREEWIDGET_H
