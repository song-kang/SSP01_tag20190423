#ifndef MPUSHBUTTON_H
#define MPUSHBUTTON_H

#include <QPushButton>
#include <QTableWidgetItem>
class mPushButton : public QPushButton
{
	Q_OBJECT

public:
	mPushButton(QWidget *parent=0);
	virtual ~mPushButton();
	void setRowAndColumn(QTableWidgetItem* item,int column)//设置在tablewidget中的行和列
	{
		m_tableItem = item;
		m_column = column;
	}
	void setName(QString name) {
		m_strName = name;
	}
	QString name() const {
		return m_strName;
	}
	void setColID(int id) {
		m_nID = id;
	}
	int colID() const {
		return m_nID;
	}
signals:
	void whitch(QTableWidgetItem*,int);
public slots:
	void onPressed()
	{
		emit whitch(m_tableItem,m_column);
	}
private:
	int m_row;//在tablewidget中的行
	int m_column;//在tablewidget中的列
	QTableWidgetItem* m_tableItem;
	QString m_strName;
	int m_nID;
};

#endif // MPUSHBUTTON_H
