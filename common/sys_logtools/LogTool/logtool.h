/********************************************************************
	created:	2015/07/31
	filename: 	logtool.h
	author:		yws
	purpose:	Ö÷½çÃæ
*********************************************************************/
#ifndef LOGTOOL_H
#define LOGTOOL_H

#include <QtGui/QMainWindow>
#include <QtCore>
#include "ui_logtool.h"
#include "logtable.h"
#include <QStyleFactory>
#include <logabout.h>

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN 
#include <stdio.h> 
#include <stdlib.h> 
#include <winsock2.h> 

#pragma comment(lib,"ws2_32.lib")
#else
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <unistd.h>
#endif

#ifndef SOCKET 
#define SOCKET int
#endif

#ifndef NULL 
#define NULL 0
#endif

#ifndef INVALID_SOCKET
#define INVALID_SOCKET -1
#endif

#ifndef SOCKET_ERROR
#define SOCKET_ERROR -1
#endif

#ifndef BYTE
#define BYTE unsigned char
#endif

#define MAX_PACKAGE_SIZE 1020

struct myLog;
class acceptThread;
class recvThread;
class LogTool : public QMainWindow
{
	Q_OBJECT

public:
	LogTool(QWidget *parent = 0, Qt::WFlags flags = 0);
	~LogTool();
		
	void setServer(int port);
	QList<QString> *getTabNameList();
	SOCKET getSocket();
	logTable *getTableFromTab(int count);
	
protected:
	virtual void closeEvent(QCloseEvent *event);
private:
	SOCKET m_hSocket;
	int port;
	Ui::LogToolClass ui;
	QActionGroup *actionGroup;
	acceptThread *aptThread;
	QList<QString>*tabNameList;
	
private slots:
	void addTabTable(LogTool *pTool,logTable* &pTable,QString name);
	void removeSubTab(int index);
	void openFile();
	void triggered_actionAbout();
	void triggered_actionWindows();
	void triggered_actionWindowsXP();
	void triggered_actionWindowsVista();
	void triggered_actionMotif();
	void triggered_actionCDE();
	void triggered_actionPlastique();
	void triggered_actionCleanlooks();
};
////////////////////////////////////////////////////////////////////
class acceptThread: public QThread {

public:
	void getpTool(LogTool *point);
	virtual void run();
	
private:
	~acceptThread();
	LogTool *pTool;
	QList<recvThread*> threadList;
};

class recvThread: public QThread {

	Q_OBJECT;
public:
	void getpTool(LogTool *point);
	void getIp(QString Ip);
	void getSock(SOCKET sock);
	virtual void run();
	~recvThread();
private:
	LogTool *pTool;
	QString ip;
	SOCKET sockConn;
	QString name;

signals:
	void createTab(LogTool *pTool,logTable* &pTable,QString name);
	void setRow(QList<stuLog *> logs);
};
#endif // LOGTOOL_H
