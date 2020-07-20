#ifndef MSGREPLAY_H
#define MSGREPLAY_H

#include <QWidget>
#include "ui_msgreplay.h"
#include <QDialog>
#include <QtGui>
#include <QtCore>
#include "ssp_baseview.h"
#include "ssp_database.h"
#include "ssp_datawindow.h"
#include "SRawSocket.h"
#include "SSvgWnd.h"
#include "GuiApplication.h"
#include "PcapFile.h"

#ifdef WIN32
#ifdef NDEBUG
#pragma comment(lib,"sbase_rawsock.lib")
#else
#pragma comment(lib,"sbase_rawsockd.lib")
#endif
#endif

class MsgReplay_taskdlg;
struct MsgReplayStruct
{
	int sendType;
	QString port;
	QString filePath;
};
class MsgReplayThread:public QObject
{
		Q_OBJECT
public:
	MsgReplayThread(int no,QWaitCondition *con,MsgReplayStruct *msg,QObject *parent);
	~MsgReplayThread();

	
private:
	
	QMutex mutex;
	int no;
	bool isStop;
	QWaitCondition *con;
	MsgReplayStruct *msg;
	QObject *parent;
signals:
	void msgNum(int no,int count,int byteCount,int ts);
private slots:
	void setsleep(int no);
	void setWake(int no);
	void run(int no);
	void stopThread(int no);
	void finished ();

};
/////////////////////////////////////////////////

struct MsgThreadStruct
{
	int no;
	QWaitCondition *con;
	QThread *thread;
	MsgReplayThread *replayThread;
};
class MsgReplay : public CBaseView
{
	Q_OBJECT

public:
	MsgReplay(QWidget *parent = 0);
	~MsgReplay();

private:
	Ui::MsgReplay ui;
	QList<MsgThreadStruct> m_listmsg;
	MsgReplay_taskdlg *m_dlg;
	int getNoMax();
signals:
	void setSleep(int no);
	void setWake(int no);
	void operate(int no);
	void stopThread(int no);
private slots:
	void onpushButton_add();
	void onpushButton_del();
	void onBtn_continue();
	void onBtn_wait();
	void onBtn_del();
	void refreshMsgNum(int no,int count,int byteCount,int ts);
};

////////////////////////////////////////////////////
class MsgReplay_taskdlg: public QDialog
{
	Q_OBJECT
public:
	MsgReplay_taskdlg(QWidget *parent = 0);
	~MsgReplay_taskdlg();
	MsgReplayStruct* getMsg();
private:
	QLabel *sendLabel;
	QComboBox *sendCbo;
	QLabel *inputNumName;
	QLineEdit *inputNum;
	QLabel *fileLabel;
	QLineEdit *fileLine;
	QPushButton *filePath;
	QLabel *portLabel;
	QComboBox *portCbo;
	QPushButton *Ok;
	QPushButton *cancel;
	QString filePathName;
	QString port;
	QString sendType;
	MsgReplayStruct *msg;
private:
	void initDlg();
	
	
private slots:
	void onsendCboChanged(QString text);
	void onportCboChanged(QString text);
	void onfilePath();
	void onOk();
	void onCancel();
};
#endif // MSGREPLAY_H
