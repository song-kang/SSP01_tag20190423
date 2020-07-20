/**
 *
 * 文 件 名 : mcombobox.h
 * 创建日期 : 2015-7-30 15:01
 * 作    者 : 褚冰冰
 * 修改日期 : 2015-7-30 15:01
 * 当前版本 : 1.0
 * 功能描述 : 
 * 修改记录 : 
 *            $Log: $
 *
 * Ver  Date        Author  Comments
 * ---  ----------  ------  -------------------------------------------
 * 001	2015-7-30 	褚冰冰　重写QCombobox控件
 *
 **/

#ifndef MCOMBOBOX_H
#define MCOMBOBOX_H
#include "SObject.h"
#include <QComboBox>
#include <QTreeWidgetItem>
#include <QTableWidgetItem>
//////////////////////////////////////////////////////////////////////////
// 名    称:  mCombobox
// 作    者:  褚冰冰
// 创建时间:  2015-7-30 15:02
// 描    述:  重写QCombobox控件，添加部分功能
//////////////////////////////////////////////////////////////////////////
class mCombobox : public QComboBox
{
	Q_OBJECT

public:
	mCombobox(QWidget *parent=0,int type=0);
	virtual ~mCombobox();

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  设置在tablewidget中的行和列
	// 作    者:  褚冰冰
	// 创建时间:  2015-7-30 15:02
	// 参数说明:  @row 行
	//         :  @column 列
	// 返 回 值:  
	//////////////////////////////////////////////////////////////////////////
	void setRowAndColumn(QTableWidgetItem *item,int column)//设置在tablewidget中的行和列
	{
		m_tableItem = item;
		m_column = column;
	}
	void setTreeItemAndColumn(QTreeWidgetItem*item,int column)
	{
		m_treeItem=item;
		m_column=column;
	}
signals:
	void whitch(QTableWidgetItem *,int);
	void whitch(QTreeWidgetItem*,int);
public slots:
	//////////////////////////////////////////////////////////////////////////
	// 描    述:  选择变化时触发槽函数(Qtablewidget中)
	// 作    者:  褚冰冰
	// 创建时间:  2015-7-30 15:04
	// 参数说明:  @index
	// 返 回 值:  
	//////////////////////////////////////////////////////////////////////////
	void changeToWidget(int index);

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  选择变化时触发槽函数（QTreeWidget中）
	// 作    者:  褚冰冰
	// 创建时间:  2015-7-30 15:04
	// 参数说明:  @index
	// 返 回 值:  
	//////////////////////////////////////////////////////////////////////////
	void changeToTree(int index);
private:
	int m_row;//在tablewidget中的列
	int m_column;//在tablwidget中的行
	int m_type;//类型：0:在tablewidget中 1：在treewidget中
	QTreeWidgetItem * m_treeItem;
	QTableWidgetItem* m_tableItem;
};

#endif // MCOMBOBOX_H
