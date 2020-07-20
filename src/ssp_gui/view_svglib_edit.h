#ifndef VIEW_SVGLIB_EDIT_H
#define VIEW_SVGLIB_EDIT_H

#include <QWidget>
#include "ui_view_svglib_edit.h"
#include "ssp_baseview.h"
#include "ssp_database.h"
#include "SSvgWnd.h"
#include "extattributedlg.h"
class SvgOutputDlg;
class inputNum1;
class view_svglib_edit : public CBaseView
{
	Q_OBJECT

public:
	view_svglib_edit(QWidget *parent = 0);
	~view_svglib_edit();
	static void svgFiletoXmlTxt();
private:
	void refreshTypeTable();
private:
	Ui::view_svglib_edit ui;
	SDatabaseOper *pDb;//数据窗口管理对象
	SRecordset m_typeTable;//图元类型信息
	SRecordset m_svgTable;//图元信息
	DynamicSSvgEditorWnd *m_svgEditWnd;//svg编辑对话框
	inputNum1 *m_inputDlg;
private slots:
	void onPushButton_add();
	void onPushButton_edit();
	void onPushButton_del();
	void onPushButton_copy();
	void onPushButton_up();
	void onPushButton_down();
	void onPushButton_add_svg();
	void onPushButton_edit_svg();
	void onPushButton_del_svg();
	void onPushButton_copy_svg();
	void onPushButton_up_svg();
	void onPushButton_down_svg();
	void onPushButton_othtype_svg();
	void onPushButton_import();
	void onPushButton_export();
	void onPushButton_app();
	void editType(QTableWidgetItem *item);
	void editSvg(QTableWidgetItem *item);
	void showSvgInfo(QTableWidgetItem *item,QTableWidgetItem *pre);
};
////////////////////////////////////////////////////////
class SvgOutputDlg : public QDialog
{
	Q_OBJECT

public:
	SvgOutputDlg(QWidget *parent = 0);
	~SvgOutputDlg();

private:
	SDatabaseOper *pDb;
	SRecordset m_svgType;//主表信息
	SRecordset m_svg;//svg数
	QMap<int,QTreeWidgetItem*>rootMap;
	void setupUi(QDialog *SvgOutputDlg);
	void retranslateUi(QDialog *SvgOutputDlg);
private:
	QGridLayout *gridLayout_2;
	QVBoxLayout *verticalLayout;
	QGroupBox *groupBox;
	QGridLayout *gridLayout;
	QTreeWidget *treeWidget;
	QHBoxLayout *horizontalLayout_3;
	QHBoxLayout *horizontalLayout_2;
	QPushButton *pushButton;
	QPushButton *pushButton_2;
	QSpacerItem *horizontalSpacer;
	QHBoxLayout *horizontalLayout;
	QPushButton *pushButton_3;
	QPushButton *pushButton_4;
	private slots:
		void onPushButtonAllChecked();//全选
		void onPushButtonAllUnchecked();//全不选
		void onPushButtonOk();//确定
		void onPushButtonCancel();//取消
		void tree_checked(QTreeWidgetItem *item);
};
//////////////////////////////////////////////////////////
/////////////////////////////////////////

class inputNum1 : public QDialog
{
	Q_OBJECT

public:
	inputNum1(QWidget *parent = 0);
	~inputNum1();
	int getNum();
	bool isOk();
private:
	bool m_ok;
	SDatabaseOper *pDb;
	SRecordset m_sets;
	QLabel *label;
	QComboBox *cbo;
	QPushButton *pushButton;
	QPushButton *pushButton2;
signals:
	void finishInputNum();
	private slots:
		void onPushButtonPressed();
		void onPushButton2Pressed();
};
/////////////////////////////////////////
#endif // VIEW_SVGLIB_EDIT_H
