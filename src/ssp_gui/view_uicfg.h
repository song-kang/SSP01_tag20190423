#ifndef VIEW_UICFG_H
#define VIEW_UICFG_H

#include <QWidget>
#include "ui_view_uicfg.h"
#include "ssp_baseview.h"
#include "ssp_datawindow.h"
#include "mtreewidget.h"
#include "mcombobox.h"
#include "mpushbutton.h"
#include "record_sql.h"
#include "clearData.h"
#include "ssp_gui.h"
#include "extattributedlg.h"

class view_uicfg;
////////////////////  class UicfgWndDelegate  /////////////////////////
class UicfgWndDelegate : public QItemDelegate
{
	Q_OBJECT

public:
	UicfgWndDelegate(QObject *parent, int no);
	~UicfgWndDelegate();

	QWidget * createEditor(QWidget * parent, const QStyleOptionViewItem & option, const QModelIndex & index) const;
	void updateEditorGeometry(QWidget *editor,const QStyleOptionViewItem &option, const QModelIndex & index) const;
	void setEditorData(QWidget *editor,const QModelIndex &index) const;
	void setModelData(QWidget *editor,QAbstractItemModel *model,const QModelIndex &index) const;
private:
	view_uicfg *parent;
	SDatabaseOper *pDb;
	int colno;
};
struct stUicfgWnd
{
	QString wnd_sn;
	QString wnd_name;
	QString create_author;
	QString create_time;
	QString modify_author;
	QString modify_time;
	QString wnd_type;
	QString svg_file;
	QString dataset;
	QString ds_cond;
	QString refresh_sec;
	QString cls_name;
};
///对应系统管理-》功能管理-》组态窗口配置功能点
class view_uicfg : public CBaseView
{
	Q_OBJECT

public:
	view_uicfg(QWidget *parent = 0);
	~view_uicfg();
	bool InitWidget(SString sWname);//初始化窗口
public slots:
	void on_pushButton_pressed();//新增
	void on_pushButton_2_pressed();//删除
	void on_tableWidget_itemDoubleClicked(QTableWidgetItem * item);
	void on_tableWidget_itemChanged(QTableWidgetItem * item);
	void on_tableWidget_itemSelectionChanged();
	void on_tableWidget_currentItemChanged ( QTableWidgetItem * current, QTableWidgetItem * previous );
	int getMaxIndex();
	void OnPreShow();
	void onPushButton_copy();
	void initCombo();
	//20160815 林德智
public slots:
	void refreshTable(QString strClsName);
public:
	void initTable();
	//刷新数据集的名称和描述映射关系
	void refreshDataAndReportSetMap();
	//插入一行数据
	void insertNewRow(stUicfgWnd* stUW,int curRow);
	//初始化所有基本数据的映射关系
	void initDataInfoMap();
	QString getItemValue(int row, int col);
	int	getRowByPushButton(QObject *object);
	void addTableWidgetOperItem(int row);

	bool getCheckBoxStateAndComBoxText(QString& strComBoxText);
	QMap<QString,QString> reportMap;//报表及序号映射关系
	QMap<QString,QString> wndTypeMap;//窗口类型映射关系
	QMap<QString,QString> dataSetMap;//数据集对象的名称和描述映射关系

private slots:
	void slotupLoadRowClicked();
	void slotDownLoadRowClicked();
	void slotEditorRowClicked();
private:
	Ui::view_uicfg ui;
	SString m_tableName;//表名
	int m_tableRow;//不算上插入行的行数
	SDatabaseOper *m_dbOper;//数据窗口管理对象
	QWidget * m_widget;
	SRecordset* m_Record;
	record_sql m_sqlRecord;//命令集
	ssp_gui* m_sspgui;
	DynamicSSvgEditorWnd * m_svgEditWnd;//svg编辑对话框
	bool eventFilter(QObject *obj, QEvent *event);//事件过滤
};

#endif // VIEW_UICFG_H
