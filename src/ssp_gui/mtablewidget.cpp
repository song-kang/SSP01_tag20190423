#include "mtablewidget.h"

mTableWidget::mTableWidget(QWidget *parent)
	: QTableWidget(parent)
{

}

mTableWidget::~mTableWidget()
{

}
void mTableWidget::setColumCellType(int column,CELLTYPE type)
{
	if (column>=(int)m_vCellInfo.size())
	{
		return;
	}
	//m_vCellInfo[column].m_column = column;
	m_vCellInfo[column].m_type = type;
}
void mTableWidget::setColumnCount(int columns)
{
	m_vCellInfo.clear();
	for (int i = 0;i<columns;i++)
	{
		CELLINFO a = {NONE,i};
		m_vCellInfo.push_back(a);
	}
	QTableWidget::setColumnCount(columns);
}