/**
 *
 * 文 件 名 : mtablewidget.h
 * 创建日期 : 2015-7-30 15:18
 * 作    者 : 褚冰冰
 * 修改日期 : 2015-7-30 15:18
 * 当前版本 : 1.0
 * 功能描述 : 重写QTableWidget
 * 修改记录 : 
 *            $Log: $
 *
 * Ver  Date        Author  Comments
 * ---  ----------  ------  -------------------------------------------
 * 001	2015-7-30 	褚冰冰　重写QTableWidget
 *
 **/

#ifndef MTABLEWIDGET_H
#define MTABLEWIDGET_H

#include "SObject.h"
#include <QTableWidget>
#include <vector>
using namespace std;

class mTableWidget : public QTableWidget
{
	Q_OBJECT
public:
	enum CELLTYPE
	{
		NONE,
		TREEWIDGET,
		COMBBOX,
	};
	struct CELLINFO
	{
		CELLTYPE m_type;
		int m_column;
	};
public:
	mTableWidget(QWidget *parent=0);
	virtual ~mTableWidget();
	void setColumCellType(int column,CELLTYPE type);
	virtual void setColumnCount(int columns);
private:
	vector<CELLINFO> m_vCellInfo;
};

#endif // MTABLEWIDGET_H
