/**
 *
 * 文 件 名 : GuiApplication.h
 * 创建日期 : 2015-7-30 15:51
 * 作    者 : 邵凯田(skt001@163.com)
 * 修改日期 : $Date: $
 * 当前版本 : $Revision: $
 * 功能描述 : 图形界面应用程序类
 * 修改记录 : 
 *            $Log: $
 *
 * Ver  Date        Author  Comments
 * ---  ----------  ------  -------------------------------------------
 * 001	2015-7-30	邵凯田　创建文件
 *
 **/

#ifndef __TEST_GUI_APPLICATION_H__
#define __TEST_GUI_APPLICATION_H__

#include <QtGui/QApplication>
#include <QtGui/QMainWindow>
#include "SApplication.h"
#include "GuiFrame.h"
#include "ssp_gui.h"
#include "ssp_database.h"
#include "ssp_funpoint.h"
#include "ssp_datawindow.h"
#include "sp_svglib_config.h"
#include "sim_config.h"
#include "sim_manager.h"

enum eEventLevel
{
	EV_TIP=1,
	EV_WARN,
	EV_ERROR,
	EV_FAULT,
};

enum eEventType
{
	ET_SVOUT=1,
	ET_GSEOUT,
	ET_MMSSVR,
	ET_MMSCLIENT,
	ET_NTPSVR,
	ET_NTPCLIENT,
	ET_TCPSVR,
	ET_TCPCLIENT,
	ET_UDP,
	ET_ICMP,
	ET_ARP,
	ET_STORM,
	ET_SUBNET,
	ET_SYS,
	ET_UNKNOWN,
};

struct stuRealEvent
{
	SString sEventType;
	int soc;
	int usec;
	SString sIedDesc;
	SString sEventLevel;
	SString sEventInfo;
};
void InitConsoleWindow();
class CFloatWnd_RealEvent : public QWidget
{
	Q_OBJECT
public:
	CFloatWnd_RealEvent(QWidget *p=NULL) : QWidget(p)
	{
		setWindowTitle(tr("实时事件窗口"));
		m_pGridLayout = new QGridLayout(this);
		m_pGridLayout->setSpacing(6);
		m_pGridLayout->setContentsMargins(5, 5, 5, 5);
		m_pGridLayout->setObjectName(QString::fromUtf8("m_pGridLayout"));

		m_pTableWidget = new QTableWidget(p);
		m_pGridLayout->addWidget(m_pTableWidget);
		m_pTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);


		QStringList labels;
		m_pTableWidget->setColumnCount(5);
		labels << "事件类型" << "时间" << "IED描述" << "事件级别" << "事件详情"; 
		m_pTableWidget->setHorizontalHeaderLabels(labels);
		m_pTableWidget->setColumnWidth(0,80);
		m_pTableWidget->setColumnWidth(1,150);
		m_pTableWidget->setColumnWidth(2,200);		;
		m_pTableWidget->setColumnWidth(3,70);
		m_pTableWidget->setColumnWidth(4,300);
		SQt::SetTableWidgetStyle(m_pTableWidget);

		m_pPushButton = new QPushButton(NULL);
		m_pPushButton->setObjectName(QString::fromUtf8("m_pPushButton"));
		QIcon icon;
		icon.addFile(QString::fromUtf8(":/ssp_gui/Resources/MSGBOX04.ICO"), QSize(), QIcon::Normal, QIcon::Off);
		m_pPushButton->setIcon(icon);
		m_pPushButton->setCheckable(true);
		m_pPushButton->setChecked(false);
		m_pPushButton->setFlat(true);
		m_pPushButton->setText(tr("实时事件"));
		SSP_GUI->AddStatusWidget(m_pPushButton);
		m_RealEvents.setAutoDelete(true);
		connect(m_pPushButton,SIGNAL(clicked()),this,SLOT(OnBtnRealEvent()));
		m_pTimer = new QTimer();
		connect(m_pTimer,SIGNAL(timeout()),this,SLOT(OnTimer()));
		m_pTimer->start(100);

	}
	virtual ~CFloatWnd_RealEvent()
	{
		delete m_pTimer;
	}
	virtual void hideEvent ( QHideEvent * event )
	{
		QWidget::hideEvent ( event );
		if(isVisible())
			m_pPushButton->setChecked(true);
		else
			m_pPushButton->setChecked(false);
	}

	public slots:
		void OnTimer()
		{
			stuRealEvent *pEvent;
			while(m_RealEvents.count()>0)
			{
				pEvent = m_RealEvents[0];
				if(m_pTableWidget->rowCount() >= 10000)
					m_pTableWidget->removeRow(0);
				int row = m_pTableWidget->rowCount();
				m_pTableWidget->insertRow(row);
				m_pTableWidget->setItem(row,0,new QTableWidgetItem(pEvent->sEventType.data()));
				SDateTime dt = SDateTime::makeDateTime((time_t)pEvent->soc,pEvent->usec/1000,pEvent->usec%1000);
				m_pTableWidget->setItem(row,1,new QTableWidgetItem(dt.toString("yyyy-MM-dd hh:mm:ss.zzzuuu").data()));
				m_pTableWidget->setItem(row,2,new QTableWidgetItem(pEvent->sIedDesc.data()));
				m_pTableWidget->setItem(row,3,new QTableWidgetItem(pEvent->sEventLevel.data()));
				m_pTableWidget->setItem(row,4,new QTableWidgetItem(pEvent->sEventInfo.data()));
				m_RealEvents.remove(0);
			}
			m_pPushButton->setText(m_pTableWidget->rowCount()==0?"实时事件":SString::toFormat("实时事件(%d)",m_pTableWidget->rowCount()).data());
		}
		void OnBtnRealEvent()
		{
			if(isVisible())
			{
				m_pPushButton->setChecked(false);
				hide();
			}
			else
			{
				m_pPushButton->setChecked(true);
				show();
				setFocus();
			}
		}
public:
	SPtrList<stuRealEvent> m_RealEvents;
private:

	QGridLayout *m_pGridLayout;
	QTableWidget *m_pTableWidget;
	QPushButton *m_pPushButton;
	QTimer *m_pTimer;
};

class CGuiApplication : public SApplication
{
public:
	CGuiApplication(void);
	virtual ~CGuiApplication(void);

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  取APP应用实例指针
	// 作    者:  邵凯田
	// 创建时间:  2015-7-30 15:59
	// 参数说明:  void
	// 返 回 值:  CHmiApplication*
	//////////////////////////////////////////////////////////////////////////
	static CGuiApplication* GetApp();
	static QApplication* GetQApp();

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  设备QT应用类指针
	// 作    者:  邵凯田
	// 创建时间:  2015-7-30 15:59
	// 参数说明:  @pApp
	// 返 回 值:  void
	//////////////////////////////////////////////////////////////////////////
	void SetQApplication(QApplication *pApp);

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  启动服务，开启所有应用的服务，该函数必须由派生类实现，且启动服务完成后必须返回
	// 作    者:  邵凯田
	// 创建时间:  2015-7-30 15:59
	// 参数说明:  void
	// 返 回 值:  true/false
	//////////////////////////////////////////////////////////////////////////
	virtual bool Start();

	////////////////////////////////////////////////////////////////////////
	// 描    述:  停止服务，关闭所有应用的服务
	// 作    者:  邵凯田
	// 创建时间:  2015-7-30 15:59
	// 参数说明:  void
	// 返 回 值:  true/false
	//////////////////////////////////////////////////////////////////////////
	virtual bool Stop();

	////////////////////////////////////////////////////////////////////////
	// 描    述:  取当前模块的运行状态信息，对应接入模块应列出所有接入设备的状态明细
	// 作    者:  邵凯田
	// 创建时间:  2015-7-30 15:59
	// 参数说明:  void
	// 返 回 值:  SString
	//////////////////////////////////////////////////////////////////////////
	virtual SString GetNowModInfo();

	////////////////////////////////////////////////////////////////////////
	// 描    述:  代理消息处理接口，由派生类实现，处理函数必须尽量短小，快速返回
	// 作    者:  邵凯田
	// 创建时间:  2016-2-3 15:20
	// 参数说明:  @wMsgType表示消息类型
	//         :  @pMsgHead为消息头
	//         :  @sHeadStr消息头字符串
	//         :  @pBuffer为消息携带的二进制数据内容，NULL表示无二进制数据
	//         :  @iLength为二进制数据长度
	// 返 回 值:  true表示处理成功，false表示处理失败或未处理
	//////////////////////////////////////////////////////////////////////////
	virtual bool ProcessAgentMsg(WORD wMsgType,stuSpUnitAgentMsgHead *pMsgHead,SString &sHeadStr,BYTE* pBuffer=NULL,int iLength=0){return true;};

	void NewEvent(SString sEventType,int soc,int usec,SString sIedDesc,SString sEventLevel,SString sEventInfo);

	CSimConfig m_SimConfig;
	CSimManager m_SimManager;
protected:
	QApplication *m_pQApp;//QT应用指针
	CGuiFrame *m_pMainWnd;//主窗口
	CSsp_FunPointConfig *m_pFunpoint;	//功能点配置文件

	CFloatWnd_RealEvent *m_pWndRealEvent;//实时突发事件的窗口指针
	CSsp_Database *m_pDatabase;			//数据库配置文件

private:
	//关闭SCD导入时，需要操作内存库
	void closeScdToMDB();
};

#endif//__TEST_GUI_APPLICATION_H__
