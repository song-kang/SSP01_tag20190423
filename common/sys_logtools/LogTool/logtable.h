/********************************************************************
	created:	2015/07/31
	filename: 	logtable.h
	author:		yws
	purpose:	tab页面
*********************************************************************/
#ifndef LOGTABLE_H
#define LOGTABLE_H

#include <QWidget>
#include <QMessageBox>
#include <QFile>
#include <QFileDialog>
#include <QTextStream>
#include <QScrollBar>
#include "ui_logtable.h"
#include "model.h"

struct myLog;

#pragma pack(push)
#pragma pack(1)
enum LOG_LEVEL
{
	LOG_BASEDEBUG=0,
	LOG_DEBUG=1,
	LOG_WARN=2,
	LOG_ERROR=3,
	LOG_FAULT=4,
	LOG_NONE=5
};
struct stuDateTime
{
	char year;
	char month;
	char day;
	char hour;
	char minute;
	char second;
};
struct stuLog
{
	int	size;
	long	threadid;
	char	logtype;
	stuDateTime		datetime;
	char	modulename[20];
	char	filename[40];
	int		line;
#ifdef LOG_HAS_FUNCTION_NAME
	char	funname[40];
#endif
	int     sub_no;//子类型号
	char	msg[10240];//attr:m=xxx;f=xxx;t=xxx;
};

#pragma pack()
#pragma pack(pop)
class logTable : public QWidget
{
	Q_OBJECT
public:
	logTable(QWidget *parent = 0);
	~logTable();
	bool isClosed();//返回本界面是否关闭
	void setName(QString tabName);//设置页面名称
	void readFile(QString filename);//读取文件信息

private:
	Ui::logTable ui;
	bool isClose;
	QList<myLog> *logs;
	QList <myLog>*tmpLogs;
	QString tabName;
	Model *model;
signals:
	void showMsg(QString msg);
protected:
	virtual void closeEvent(QCloseEvent *event);
private slots:
	void clicked_filiterButton();
	void clicked_forwardButton();
	void clicked_backwardButton();
	void clicked_exportButton();
	void sendMsg();
	void setRow(QList<stuLog *> logs);
};

#endif // LOGTABLE_H
