#ifndef SYS_MDBUI_H
#define SYS_MDBUI_H

#define MAX_SHOW_SIZE 100000

#include <QtGui/QMainWindow>
#include <QDate>
#include <QTime>
#include <queue>
#include <QMessageBox>
#include <QFileDialog>
#include <QSettings>
#include <QTextCodec>
#include <QShortCut>
#include <QScrollBar>
#include <QTimer>
#include <QMap>
#include "ui_sys_mdbgui.h"
#include "ManConnect.h"
#include "RClickMenu.h"
#include "enterdlg.h"
#include "dbInteraction.h"
Q_DECLARE_METATYPE(sendInfo)
BYTE * registFunction(void* cbParam, SString &sTable, eMdbTriggerType eType,int iTrgRows,int iRowSize,BYTE *pTrgData);

class sys_mdbgui : public QMainWindow
{
	Q_OBJECT

public:
	sys_mdbgui(QWidget *parent = 0, Qt::WFlags flags = 0);
	~sys_mdbgui();
	bool b_append;
	//发送触发注册函数信号（直接调用setRegistInfo会引起线程崩溃）
	void sendTrigSignal()
	{
		emit setInfo();
	}
	int gettableFields(SString sTable, SPtrList<stuTableField> &TableFields)
	{
		return dbinter.getTablefields(sTable,TableFields);
	}
	dbInteraction dbinter;
	SPtrList<sendInfo> m_SendInfo;
	

signals:
	void setInfo();//设置触发显示结果信号
private:
	QString m_curTableName;//当前语句操作数据库名称（select *,create,drop 3个语法用到）
	int m_index;//当前数据库索引
	Ui::sys_mdbguiClass ui;
	bool b_save;//是否保存
	SMdb *m_mdb;
	ManConnect mc;
	Mmenu *r ;
	QTreeWidgetItem * citem;//当前选中的条目
	//dbInteraction dbinter;
	SString s_ip;
	bool b_edit;//判断tablewidget是被否被编辑
	int m_curRow;//原始列表行数
	int m_insert;//增加的行数
	bool b_connect;
	QScrollBar * bar;
	int m_TabPageRowN;//当前列表行数
	vector<QString> V_item;
	bool b_first;//是否第一次打开
	int l_reg;//注册记录字符长度
	int l_select;//查询记录字符长度
	int l_operate;//操作记录字符长度
	QTimer *m_timer;//定时器
	struct tableTofield
	{
		SString stable;
		SPtrList<stuTableField> *lf;
		//QMap<int,unsigned int> *oidMap;//oid映射
	};
	vector<tableTofield> m_vfields;//储存回调所需表字段信息
	QMap<QString,int> m_tableMap;//表名对应字段信息在m_vfields的下标
	bool m_blocal;//是否是本地操作
public slots:
void reloadDb(SString sTable,UPDATETYPE type);
void reloadDb();//刷新当前数据库列表
void refresh(SString s_table);//刷新当前表中内容
void RecordHistory(SString sSql,bool b_select);//运行sql语句历史记录 
void tableInit();
void getValue(int m_value);
void setRegistInfo();//设置触发显示结果
void on_treeWidget_itemDoubleClicked( QTreeWidgetItem * item, int column );
void on_treeWidget_customContextMenuRequested(QPoint pos);
void on_treeWidget_currentItemChanged ( QTreeWidgetItem * current, QTreeWidgetItem * previous )
{
	isSave();
	citem = current;
	if (citem==NULL)
	{
		r->setType(Mmenu::NOITEM);
		dbinter.pointTo(-1);
		return;
	}
	if(citem->childCount()==0)
	{
		r->setType(Mmenu::CHILDITEM);
		on_treeWidget_itemDoubleClicked(current,0);
	}else
	{
		m_index = ui.treeWidget->indexOfTopLevelItem(current);
		r->setType(Mmenu::FATHERITEM);
		ui.pushButton_insert->setEnabled(false);
		tableInit();
	}
}
void on_action_triggered ( bool checked = false );//执行按钮响应事件
void on_action_runfile_triggered(bool checked = false);//运行sql文件
void on_action_5_triggered ( bool checked = false );//说明按钮响应事件
void on_action_7_triggered ( bool checked = false );//帮助按钮响应事件
void on_action_6_triggered ( bool checked = false );//清空注册记录
void on_action_8_triggered ( bool checked = false );//清空查询记录
void on_action_9_triggered ( bool checked = false );//清空操作记录
void on_pushButton_insert_pressed();//插入按钮
void on_pushButton_save_pressed();//保存按钮
void on_pushButton_del_pressed();//删除按钮
void on_tableWidget_itemSelectionChanged();//框选变化
void connect();//连接
void delDb();//删除连接
void delTable();//删除表
void registCallback();//注册事件
void removeRegistCall();//删除注册事件
void registAllCallback();//全部注册事件
void removeAllRegistCall();//删除全部注册事件
void getData(SString sParam);
void execSqltoWidget(QString sql);//执行查询语句并在表中显示出来
void updateData(QTableWidgetItem * item)
{
	SString s;
	SString info;
	if (b_edit)
	{
		//修改tablewidget中的值，更新到表中
		//citem->text(0).toStdString();
		int i_row = item->row();
		int i_colum = item->column();

		if (i_row<m_curRow)
		{
			for (int i=0;i<ui.tableWidget->columnCount();i++)
			{
				s+=ui.tableWidget->item(i_row,i)->text().toLocal8Bit().data();
				s+=",";
			}
			//修改单行
			//info = dbinter.update(citem->text(0).toStdString(),s,i_colum);
			//ui.label->setText(QString::fromLocal8Bit(info.data()));
			//修改单行

			//一起修改
			dbinter.addcmd(m_curTableName.toStdString(),i_row,i_colum,s,2,s);
			ui.pushButton_save->setEnabled(true);
		}
	}
	
}
void isSave();
SString readSeting();
void test()
{
	ui.menu->exec(QPoint(ui.menuBar->geometry().x(),ui.menuBar->geometry().y()+ui.menuBar->geometry().height()));
};
void writeSeting(QString s_ip="127.0.0.1",QString s_port="1400",QString s_user="skt_mdb",QString s_password="skt001@163.com");
private:
	void setShortCut();
};

#endif // SYS_MDBUI_H
