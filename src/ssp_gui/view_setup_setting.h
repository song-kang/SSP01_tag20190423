#ifndef VIEW_SETUP_SETTING_H
#define VIEW_SETUP_SETTING_H

#include <QtGui>
#include "ui_view_setup_setting.h"
#include "ssp_baseview.h"
#include "ssp_database.h"
#include "ssp_datawindow.h"
#include "SSvgWnd.h"
#include "view_common.h"
#include "view_query.h"
#include "view_dw_condition_edit.h"
//系统管理-》功能管理-》全局配置模板界面的格式字符设置界面
class setup_setting_widget;
class view_val_fmt_dlg: public QDialog
{
	Q_OBJECT
public:
	view_val_fmt_dlg(QWidget* parent, int type, QString strFmtValue);
	~view_val_fmt_dlg();
	void setupUi(int type);
	void initIntOrFloatSet();
	void initEnumSet();
	void initRefSet();
	void initStringSet();
protected:
	//void contextMenuEvent(QContextMenuEvent * event);
private:
	void initEnumTableWidget();
	void insertOneRowToEnumTableWidget(int row, QString strIndex, QString strValue);
	int getMaxIndexFromEnumTableWidget();
	bool checkIndexIsExist(int irow, QString str);
private slots:
	void slotOnOkPushButtonClick();
	void slotAddNewRow();
	void slotcustomContextMenuRequested(const QPoint& pos);
	void slotDelOneEnum();
	void slotTableItemchanged(QTableWidgetItem *item);
private:
	int m_iValType;//1-	布尔类型;2-	枚举类型;3-	整数类型;4-	浮点类型;	5-	字符串类型;6-	引用类型
	QString m_strOldFmtValue;
	QString m_strMin;//保存整形或是浮点型最小值
	QString m_strMax;//保存整形或是浮点型最大值
	QString m_strStringMaxValue;////保存整形或是浮点型最小值
	QString m_strRef;//保存引用类型值
public:
	QString m_strShowInfo;//保存在格式字符上显示的值
private:
	QPushButton* m_okPushButton;
	QPushButton* m_cancelPushButton;
	QPushButton* m_addPushButton;
	QPushButton* m_delPushButton;
	QLineEdit* m_minLineEdit;
	QLineEdit* m_maxLineEdit;
	QLineEdit *m_strlineEdit;
	QComboBox* m_refComboBox;
	//QListWidget* m_enumListWidget;
	QTableWidget* m_enumTableWidget;
	QMenu*       m_enumSubMenu;
	QAction*     m_delEnumAction;
	SDatabaseOper *pDb;
	QMap<QString, QString> m_refMap;
	setup_setting_widget* m_pParent;
	

};
class view_setup_setting : public CBaseView
{
	Q_OBJECT

public:
	view_setup_setting(QWidget *parent = 0);
	~view_setup_setting();
	void refresh();
private:
	Ui::view_setup_setting ui;
	SDatabaseOper *pDb;
private slots:
		void on_pushButton_typeAdd();
		void on_pushButton_typeEdit();
		void on_pushButton_typeDel();
		
};
/////////////////////////////////////////////////////////////////
class setup_setting_widget:public CBaseView
{
	Q_OBJECT

	public:
		setup_setting_widget(QWidget *parent,int typeId);
		~setup_setting_widget();
		QTableWidget* getTable();
		int getTypeId();
		void refresh();
		QStringList getMinMax(QString valFmt);
		int getLenMax(QString valFmt);
		QStringList getEnum(QString valFmt);
		//获取以；为分割符的字符格式描述中的枚举值列表
		QStringList getEnumList(QString valFmt);
	private:
		QPushButton *pushButton_add;
		QPushButton *pushButton_del;
		QTableWidget *table;
		int typeId;
		void initTable();
		SDatabaseOper *pDb;
		int typeStrToValue(QString str);
		int classStrToValue(QString str);
		QString classValueToStr(int value);
		QString typeValueToStr(int value);
	
	private slots:
		void table_itemchanged(QTableWidgetItem *item);
		void on_pushButton_add();
		void on_pushButton_del();
		void slotOnCellDoubleCliced(int row, int column);
};
//////////////////////////////////////////////////////////////////////////

class setup_setting_widgetDelegate : public QStyledItemDelegate
{
	Q_OBJECT

public:
	setup_setting_widgetDelegate(QObject *parent = 0);

	virtual QWidget* createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
	virtual void setEditorData(QWidget *editor, const QModelIndex &index) const;
	virtual void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;

private:
	setup_setting_widget *parent;
	int curRow;
	int curCol;

	private slots:
		void comboItemChanged(QString text);
};
#endif // VIEW_SETUP_SETTING_H

