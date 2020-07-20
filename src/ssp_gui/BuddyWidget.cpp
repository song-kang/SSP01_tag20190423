/**
 *
 * 文 件 名 : BuddyWidget.cpp
 * 创建日期 : 2018-9-4 19:27
 * 作    者 : 邵凯田(skt001@163.com)
 * 修改日期 : $Date: $
 * 当前版本 : $Revision: $
 * 功能描述 : 用于外部伴随窗口，通过HWND句柄强制劫持窗口
 *            目标应用程序启用时，取自身的窗口句柄并通过UDP发送消息到本机的5678端口，内容为（hwnd可以为10进制和16进制，16进制时需要0x开头）：
 *            String str = "type=buddy_wnd;app_name="+app_name+";app_desc="+app_desc+";hwnd="+hwnd+";";
 * 修改记录 : 
 *            $Log: $
 *
 * Ver  Date        Author  Comments
 * ---  ----------  ------  -------------------------------------------
 * 001	2018-9-4	邵凯田　创建文件
 *
 **/
#include "BuddyWidget.h"

#ifdef WIN32

SSocket *g_pBuddyUdpListener = NULL;
SPtrList<SBuddyWidget> g_BuddyWidgetList;
SBuddyWidget::SBuddyWidget(QWidget *parent)	: QWidget(parent)
{
	g_BuddyWidgetList.append(this);
	if(g_pBuddyUdpListener == NULL)
	{
		g_BuddyWidgetList.setShared(true);
		g_pBuddyUdpListener = new SSocket();
		g_pBuddyUdpListener->CreateUdp();
		g_pBuddyUdpListener->SetTimeout(10,100);
		if(!g_pBuddyUdpListener->Bind(5678))
		{
			LOGERROR("无法打开用于伴随窗口侦听的UDP端口[5678]!");
		}
		SKT_CREATE_THREAD(ThreadUdp,0);
	}
}

SBuddyWidget::~SBuddyWidget()
{
	cleanBuddyWnd();
	g_BuddyWidgetList.remove(this);
	if(g_BuddyWidgetList.count() == 0)
	{
		SApi::UsSleep(100000);
		delete g_pBuddyUdpListener;
		g_pBuddyUdpListener = NULL;
	}
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  设置伴随应用的名称
// 作    者:  邵凯田
// 创建时间:  2018-9-4 20:14
// 参数说明:  @app_name应用程序名称，必须与目标程序完全一致
// 返 回 值:  void
//////////////////////////////////////////////////////////////////////////
void SBuddyWidget::setBuddyAppName(char *app_name)
{
	m_sAppName = app_name;
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  设置伴随窗口句柄
// 作    者:  邵凯田
// 创建时间:  2018-9-4 20:15
// 参数说明:  @hwnd为句柄
// 返 回 值:  void
//////////////////////////////////////////////////////////////////////////
void SBuddyWidget::setBuddyWnd(int hwnd)
{
	if(m_iAppHWND == hwnd)
		return;
	if(m_iAppHWND != 0)
		cleanBuddyWnd();
	m_iAppHWND = hwnd;
	SetParent((HWND)m_iAppHWND,(HWND)winId());
	onResize();
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  窗口大小改变时的事件处理，使伴随窗口跟着改变
// 作    者:  邵凯田
// 创建时间:  2018-9-4 20:17
// 参数说明:  void
// 返 回 值:  void
//////////////////////////////////////////////////////////////////////////
void SBuddyWidget::onResize()
{
	if(m_iAppHWND == 0)
		return;
	RECT r;
	GetClientRect((HWND)winId(),&r);
	MoveWindow((HWND)m_iAppHWND,r.left,r.top,r.right,r.bottom,TRUE);
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  清除伴随窗口信息
// 作    者:  邵凯田
// 创建时间:  2018-9-4 20:17
// 参数说明:  void
// 返 回 值:  void
//////////////////////////////////////////////////////////////////////////
void SBuddyWidget::cleanBuddyWnd()
{
	SetParent((HWND)m_iAppHWND,NULL);
	ShowWindow((HWND)m_iAppHWND,SW_MAXIMIZE);
	m_iAppHWND = 0;
}

void SBuddyWidget::resizeEvent( QResizeEvent * event )
{
	onResize();
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  UDP命令侦听线程
// 作    者:  邵凯田
// 创建时间:  2018-9-4 20:18
// 参数说明:  void*
// 返 回 值:  void*
//////////////////////////////////////////////////////////////////////////
void* SBuddyWidget::ThreadUdp(void *lp)
{
	char buf[1025];
	int ret;
	sockaddr_in sa;
	memset(buf,0,sizeof(buf));
	SString str;
	SString type,app_name,app_desc,hwnd;
	while(g_BuddyWidgetList.count() > 0)
	{
		ret = g_pBuddyUdpListener->RecvFrom(buf,1024,sa);
		if(ret > 0)
		{
			LOGDEBUG("RECV UDP:%s",buf);
			//String str = "type=buddy_wnd;app_name="+app_name+";app_desc="+app_desc+";hwnd="+hwnd+";";
			str = buf;
			type = SString::GetAttributeValue(str,"type");
			app_name = SString::GetAttributeValue(str,"app_name");
			app_desc = SString::GetAttributeValue(str,"app_desc");
			hwnd = SString::GetAttributeValue(str,"hwnd");
			if(type != "buddy_wnd")
			{
				SApi::UsSleep(10000);
				continue;
			}
			g_BuddyWidgetList.lock();
			unsigned long pos;
			SBuddyWidget *p = g_BuddyWidgetList.FetchFirst(pos);
			while(p)
			{
				if(p->m_sAppName == app_name)
				{
					p->setBuddyWnd(hwnd.toInt());
					break;
				}
				p = g_BuddyWidgetList.FetchNext(pos);
			}
			g_BuddyWidgetList.unlock();

			memset(buf,0,sizeof(buf));
		}
		SApi::UsSleep(10000);
	}
	return 0;
}

// 
// void test_java_inqt::onpushButton()
// {
// 	QString str = ui.lineEdit->displayText();
// 	int hwnd = str.toInt(0,16);
// 	SetParent((HWND)hwnd,(HWND)ui.widget->winId());
// }


#endif