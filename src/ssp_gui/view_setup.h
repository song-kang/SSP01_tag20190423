#ifndef VIEW_SETUP_H
#define VIEW_SETUP_H

#include "ui_view_setup.h"
#include <QtGui>
#include "ui_view_setup_setting.h"
#include "ssp_baseview.h"
#include "ssp_database.h"
#include "ssp_datawindow.h"
#include "SSvgWnd.h"
#include "view_common.h"
#include "view_query.h"
#include "view_dw_condition_edit.h"

class view_setup : public CBaseView
{
	Q_OBJECT

public:
	view_setup(QWidget *parent = 0);
	void refresh();
	~view_setup();
	virtual void OnPreShow();
private:
	Ui::view_setup ui;
	SDatabaseOper *pDb;
};
/////////////////////////////////////////////////////////////////////
class setup_widget:public CBaseView
{
	Q_OBJECT

public:
	setup_widget(QWidget *parent,int typeId);
	~setup_widget();
	QTableWidget* getTable();
	int getTypeId();
	void refresh();
	QStringList getEnum(QString valFmt);
private:
	QCheckBox *checkBox_class;
	QTableWidget *table;
	int typeId;
	void initTable();
	SDatabaseOper *pDb;
	QString classValueToStr(int value);
	QString typeValueToStr(int value);
	int typeStrToValue(QString str);
	QStringList getMinMax(QString valFmt);
	int getLenMax(QString valFmt);
private slots:
	void table_itemchanged(QTableWidgetItem *item);
	void on_checkBox_class(int state);
};
//////////////////////////////////////////////////////////////////////////

class setup_widgetDelegate : public QStyledItemDelegate
{
	Q_OBJECT

public:
	setup_widgetDelegate(QObject *parent = 0);

	virtual QWidget* createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
	virtual void setEditorData(QWidget *editor, const QModelIndex &index) const;
	virtual void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;

private:
	setup_widget *parent;
	int curRow;
	int curCol;

	private slots:
		void comboItemChanged(QString text);
};
#endif // VIEW_SETUP_H
