#ifndef VIEW_DATAWINDOW_EDIT_H
#define VIEW_DATAWINDOW_EDIT_H

#include <QWidget>
#include "ui_view_datawindow_edit.h"
#include <QtGui>
#include "ssp_baseview.h"
#include "ssp_database.h"
#include "ssp_datawindow.h"
#include "SSvgWnd.h"
#include "view_common.h"
#include "view_query.h"
#include "view_dw_condition_edit.h"
class inputNum;
class review;
struct myRow
{
	SString name;
	SString id;
};
struct myRef
{
	SString name;
	SString dbtype;
	SString dw_desc;
	SString dw_select;
	SString cls_name;
	SPtrList<myRow> myRowList;
};
struct myCondition
{
	SString con_name;
	int is_option;
	int cmp_type;
	int input_type;
	SString ref_name;
	SString def_value;
	SString dyn_where;
};
struct myCol
{
	SString col_name;
	SString col_vtype;
	int col_width;
	SString ref_name;
	SString conv;
	int pkey;
	int isnull;
	int ukey;
	SString ext_attr;
};
struct myDataset
{	
	int dstype;
	SString name;
	int dbtype;
	SString dw_desc;
	SString dw_select;
	int transform_type;
	SString cross_refname;
	int cross_column;
	int cross_data_column;
	SString cls_name;
	SString ext_attr;
	SPtrList<myCol> myColList;
	SPtrList<myCondition> myConditionList;
};
class view_datawindow_edit : public CBaseView
{
	Q_OBJECT

public:
	view_datawindow_edit(QWidget *parent = 0);
	~view_datawindow_edit();

friend class dataWindowDelegate;
friend class leftTableRefWindowDelegate;
friend class leftTableDataWindowDelegate ;
private:
	SDatabaseOper *pDb;
	void initRefCombo();
	void initDataCombo();
	Ui::view_datawindow_edit ui;
	SRecordset m_rsLeftTable;//主表信息
	SRecordset m_rsCol;//数据集列数
	SRecordset m_rsRow;//引用行数
	void refreshLeftTable();//刷新主表
	SString typeTransform(SString str);//转换类型字符
	SString valuetypeTransform(SString str);//转换类型字符
	SString refnameTransform(SString str,SRecordset &datasets );//转换引用名称字符
	SString tranftranftype(SString str);
	SString convStrToName(SString str);
	SString convNameToStr(SString name);
	//获取t_ssp_data_windows表中dw_sn字段的最大值
	int getMaxDwSn();
	//处理一条导入的引用数据
	bool handleOneRefNode(int& maxDwsn, myRef* curRefSet);
	//插入一条完整的引用数据到数据库
	bool insertOneRefSetInfo(int maxDwsn, myRef* curRefSet);
	//处理一条导入的数据集数据
	bool handleOneDataNode(int& maxDwsn, myDataset* curDataSet);
	//插入一条完整的数据集数据到数据库
	bool insertOneDataSetInfo(int maxDwsn, myDataset* curDataSet);
	QTableWidget  *getTableData2();
	QTableWidget  *getTableData1();
	QTableWidget  *getTableRef1();
	QString getCls();
	void getComboxAndCheckState(QString& strComboText, bool& bIsChecked);

	int m_countMax;//用于记录sn，每次都取最大值，确保sn值唯一
	char* checkNull(char* num);//检查是否为空
	inputNum *m_inputDlg;
	view_dw_condition_edit *viewDwConEdit;
private:
	//设置引用页的table和qtexteditor控件是否阻塞信号
	void setRefBlockSignals(bool blcok);
	//设置数据集页的table和qtexteditor控件是否阻塞信号
	void setDataBlockSignals(bool blcok);

private slots:
	void onPushButton_add();
	void onPushButton_copy();
	void onPushButton_del();
	void onPushButton_view();
	void onPushButton_import();
	void onPushButton_export();
	void onPushButton_apply();

	void onPushButton_ref2_add();
	void onPushButton_ref2_del();
	void onPushButton_data2_add();
	void onPushButton_data2_del();
	void onPushButton_newcol();

	void showRefInfo(QTableWidgetItem *item,QTableWidgetItem *pre);
	void showDataInfo(QTableWidgetItem *item,QTableWidgetItem *pre);
	void showConInfo(QTableWidgetItem *item,QTableWidgetItem *pre);
	void ref2ItemCilcked(QTableWidgetItem *item,QTableWidgetItem *pre);
	void data2ItemCilcked(QTableWidgetItem *item,QTableWidgetItem *pre);
	void editRef1(QTableWidgetItem *item);
	void editRef2(QTableWidgetItem *item);
	void editData1(QTableWidgetItem *item);
	void editData2(QTableWidgetItem *item);
	void editRefText();
	void editDataText();
	void refCombo(QString text);
	void dataCombo(QString text);

	void pasteCfg();
	void tabChanged(int num);
	void slotOnCheckBoxClick();
};
////////////////////////////////////////////////////

//对应系统管理-》数据窗口配置-》“导出”按钮响应弹出的对话框
class OutputCfg : public QDialog
{
	Q_OBJECT

public:
	OutputCfg(QWidget *parent = 0);
	~OutputCfg();
private:
	QGridLayout *gridLayout_2;
	QVBoxLayout *verticalLayout;
	QTableWidget *tableWidget;
	QWidget *widget;
	QGridLayout *gridLayout;
	QPushButton *pushButton;
	QPushButton *pushButton_2;
	QSpacerItem *horizontalSpacer;
	QGridLayout *gridLayout1;
	QGridLayout *gridLayout2;
private:
	SDatabaseOper *pDb;
	SRecordset m_rsLeftTable;//主表信息
	SRecordset m_rsCol;//数据集列数
	SRecordset m_rsRow;//引用行数
	SRecordset m_rsCondition;//条件行集

	SString tranftype(SString str);//界面
	SString tranfdbtype(SString str);
	SString tranftranftype(SString str);
	void setupUi(QWidget *OutputCfg);//布局
	void retranslateUi(QWidget *OutputCfg);
private slots:
	void onPushButtonPressed();//确定
	void onPushButton2Pressed();//取消
	void DoubleClickedTable(int col);//勾选全选
};

//////////////////////////////////////////////////////////////////////////

class dataWindowDelegate : public QStyledItemDelegate
{
	Q_OBJECT

public:
	dataWindowDelegate(QObject *parent = 0);

	virtual QWidget* createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
	virtual void setEditorData(QWidget *editor, const QModelIndex &index) const;
	virtual void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;

private:
	view_datawindow_edit *parent;
	int curRow;
	int curCol;
	SDatabaseOper *pDb;

	private slots:
		void comboItemChanged(QString text);
};

/////////////////////////////////////////

class leftTableRefWindowDelegate : public QStyledItemDelegate
{
	Q_OBJECT

public:
	leftTableRefWindowDelegate(QObject *parent = 0);

	virtual QWidget* createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
	virtual void setEditorData(QWidget *editor, const QModelIndex &index) const;
	virtual void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;

private:
	view_datawindow_edit *parent;
	int curRow;
	int curCol;
	private slots:
		void comboItemChanged(QString text);
};

/////////////////////////////////////////

class leftTableDataWindowDelegate : public QStyledItemDelegate
{
	Q_OBJECT

public:
	leftTableDataWindowDelegate(QObject *parent = 0);

	virtual QWidget* createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
	virtual void setEditorData(QWidget *editor, const QModelIndex &index) const;
	virtual void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;

private:
	view_datawindow_edit *parent;
	int curRow;
	int curCol;
	private slots:
		void comboItemChanged(QString text);
};

/////////////////////////////////////////
//对应系统管理-》数据窗口配置-》“复制”按钮响应弹出的输入对话框
class inputNum : public QDialog
{
	Q_OBJECT

public:
	inputNum(QWidget *parent = 0);
	~inputNum();
	int getNum();

private:
	QGridLayout *gridLayout;
	QVBoxLayout *verticalLayout;
	QHBoxLayout *horizontalLayout_2;
	QLabel *label;
	QLineEdit *lineEdit;
	QWidget *widget;
	QHBoxLayout *horizontalLayout;
	QSpacerItem *horizontalSpacer;
	QPushButton *pushButton;
	QPushButton *pushButton_2;
private:
	void setupUi(QDialog *inputNum);
	void retranslateUi(QDialog *inputNum);
signals:
	void finishInputNum();
private slots:
	void onPushButtonPressed();
	void onPushButton2Pressed();
};
/////////////////////////////////////////
//对应系统管理-》数据窗口配置-》“预览”按钮响应弹出的对话框
class review : public QDialog
{
	Q_OBJECT

public:
	review(QWidget *parent = 0);
	~review();
	QGridLayout *gridLayout;
	QTabWidget *tabWidget;
	QWidget *widget;
	QGridLayout *gridLayout_3;
	QGridLayout *gridLayout_2;
	QWidget *widget_2;
	QGridLayout *gridLayout_5;
	QGridLayout *gridLayout_4;
private:
	void setupUi(QDialog *review);
	void retranslateUi(QDialog *review);
};
#endif // VIEW_DATAWINDOW_EDIT_H
