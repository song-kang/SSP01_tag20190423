/**
 *
 * 文 件 名 : BuddyWidgetServer.h
 * 创建日期 : 2018-9-5 8:55
 * 作    者 : 邵凯田(skt001@163.com)
 * 修改日期 : $Date: $
 * 当前版本 : $Revision: $
 * 功能描述 : QT作为Java伴随窗口使用时的QT侧应用服务端,仅适用于WIN32平台
 * 修改记录 : 
 *            $Log: $
 *
 * Ver  Date        Author  Comments
 * ---  ----------  ------  -------------------------------------------
 * 001	2018-9-5	邵凯田　创建文件
 *
 **/

#ifndef __SKT_BUDDY_WIDGET_SERVER_H__
#define __SKT_BUDDY_WIDGET_SERVER_H__

#include "SService.h"
#include "SSocket.h"
#include "SString.h"
#include "SList.h"
#include <QtGui/QWidget>
#include "ssp_gui.h"
#include "ssp_gui_inc.h"
class BuddyMsgWidget;

class SSP_GUI_EXPORT SBuddyWidgetServer : public SService
{
public:
	struct stuWidgetInfo
	{
		SString widget_name;
		int widget_oid;
		int x,y,width,height;
		QWidget *pWidget;
	};

	SBuddyWidgetServer();
	~SBuddyWidgetServer();

	inline void setUdpListenPort(int port){m_iUdpPort = port;};


	////////////////////////////////////////////////////////////////////////
	// 描    述:  启动服务,虚函数，派生函数中必须先调用此函数
	// 作    者:  邵凯田
	// 创建时间:  2018-9-5 8:55
	// 参数说明:  void
	// 返 回 值:  true表示服务启动成功,false表示服务启动失败
	//////////////////////////////////////////////////////////////////////////
	virtual bool Start();

	////////////////////////////////////////////////////////////////////////
	// 描    述:  停止服务，派生函数中必须先调用此函数
	// 作    者:  邵凯田
	// 创建时间:  2018-9-5 8:55
	// 参数说明:  void
	// 返 回 值:  true表示服务停止成功,false表示服务停止失败
	//////////////////////////////////////////////////////////////////////////
	virtual bool Stop();

	void ProcessMsg(SString &str);
	stuWidgetInfo* SearchWidgetInfo(SString name);

private:
	//////////////////////////////////////////////////////////////////////////
	// 描    述:  UDP服务线程
	// 作    者:  邵凯田
	// 创建时间:  2018-9-5 9:01
	// 参数说明:  void
	// 返 回 值:  void
	//////////////////////////////////////////////////////////////////////////
	static void* ThreadUdp(void* lp);

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  SVG对象点击的通用动作
	// 作    者:  邵凯田
	// 创建时间:  2018-9-11 20:58
	// 参数说明:  void
	// 返 回 值:  int
	//////////////////////////////////////////////////////////////////////////
	static int QAct_SvgClicked();


	//////////////////////////////////////////////////////////////////////////
	// 描    述:  视频联动通用动作，会话属性:assdev_no/cam_pos/rec_secs/filename
	// 作    者:  邵凯田
	// 创建时间:  2018-9-11 20:58
	// 参数说明:  void
	// 返 回 值:  int
	//////////////////////////////////////////////////////////////////////////
	static int QAct_CameraLinkage();


	//////////////////////////////////////////////////////////////////////////
	// 描    述:  发送数据到JAVA
	// 作    者:  邵凯田
	// 创建时间:  2018-9-13 16:58
	// 参数说明:  @sMsg为消息内容 
	// 返 回 值:  void
	//////////////////////////////////////////////////////////////////////////
	static void SendMsgToJava(SString sMsg);

	int m_iUdpPort;//UDP端口号，缺省为5678
	SSocket m_udp;
	SPtrList<stuWidgetInfo> m_widgets;
	BuddyMsgWidget *m_pMsgWidget;//用于界面消息处理的窗口
};

class SSP_GUI_EXPORT BuddyMsgWidget : public QWidget
{
	Q_OBJECT

public:
	BuddyMsgWidget(QWidget *parent = 0);
	~BuddyMsgWidget();
	void setServer(SBuddyWidgetServer *p){m_pServer = p;};
	void onMsg(QString str)
	{
		emit sigMsg(str);
	}
signals:
	void sigMsg(QString str);

	public slots:
		void sltMsg(QString str);

private:
	SBuddyWidgetServer *m_pServer;
};

#endif//__SKT_BUDDY_WIDGET_SERVER_H__
