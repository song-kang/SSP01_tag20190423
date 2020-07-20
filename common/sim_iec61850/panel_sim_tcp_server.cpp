#include "panel_sim_tcp_server.h"
#include "panel_sim_tcp_client.h"
panel_sim_tcp_server::panel_sim_tcp_server(QWidget *parent)
	: CBaseView(parent)
{
	ui.setupUi(this);
	//m_bExitHandleClientRecvThread = false;
	m_bExitTcpListenThread = false;
	init();
	connect(ui.startPushButton,SIGNAL(clicked()),this,SLOT(OnSlotStart()));
	connect(ui.stopPushButton,SIGNAL(clicked()),this,SLOT(OnSlotStop()));
	connect(this,SIGNAL(doAddHandle(SSocket* )),this,SLOT(OnSlotADD(SSocket* )));
}

panel_sim_tcp_server::~panel_sim_tcp_server()
{
	OnSlotStop();
}

void panel_sim_tcp_server::init()
{
	ui.label->hide();
	ui.ipLineEdit->hide();
#if 0
	QRegExp rx("((?:(?:25[0-5]|2[0-4]\\d|((1\\d{2})|([1-9]?\\d)))\\.){3}(?:25[0-5]|2[0-4]\\d|((1\\d{2})|([1-9]?\\d))))");
	QValidator *validatorIp=new QRegExpValidator(rx,ui.ipLineEdit);
	ui.ipLineEdit->setValidator(validatorIp);
	ui.ipLineEdit->setToolTip("请输入标准网络IP地址格式");
	ui.ipLineEdit->setText("127.0.0.1");
#endif
	QValidator *validatorPort=new QIntValidator(0,65535,ui.portLineEdit);
	ui.portLineEdit->setValidator(validatorPort);
	ui.portLineEdit->setToolTip("有效值范围0-65535");
	ui.portLineEdit->setText("7788");
	ui.stopPushButton->setEnabled(false);
	ui.startPushButton->setEnabled(true);
}

void panel_sim_tcp_server::OnSlotStart()
{
#if 0
	QString strIp = ui.ipLineEdit->text();
	if (strIp.isEmpty())
	{
		QMessageBox::warning(this,"警告","服务器IP地址不能为空！");
		return;
	}
	QRegExp rx("((?:(?:25[0-5]|2[0-4]\\d|((1\\d{2})|([1-9]?\\d)))\\.){3}(?:25[0-5]|2[0-4]\\d|((1\\d{2})|([1-9]?\\d))))");
	bool bIsMatch = rx.exactMatch(strIp);
	if (!bIsMatch)
	{
		QMessageBox::warning(this,"警告","服务器IP地址不正确，请检查IP！");
		return;
	}
	m_strIP = strIp;
#endif
	QString strPort = ui.portLineEdit->text();
	if (strPort.isEmpty())
	{
		QMessageBox::warning(this,"警告","服务器端口号不能为空！");
		return;
	}

	OnSlotStop();
	m_bExitTcpListenThread = false;
	m_strPort = strPort;
	if (!createServerSocket())
	{
		return;
	}

#ifdef WIN32
	HANDLE h; 
	h = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)(int)ThreadTcpListen, (LPVOID)this, 0, 0); 
	::SetThreadPriority(h,THREAD_PRIORITY_HIGHEST);  
	CloseHandle(h);
#else
	SKT_CREATE_THREAD(ThreadTcpListen,this);
#endif
	//SKT_CREATE_THREAD(ThreadTcpListen,this);
	ui.startPushButton->setEnabled(false);
	ui.stopPushButton->setEnabled(true);
	
}

//消息通道侦听线程
void* panel_sim_tcp_server::ThreadTcpListen(void *lp)
{
	panel_sim_tcp_server* pThis = (panel_sim_tcp_server*)lp;
	if (pThis == NULL)
	{
		LOGFAULT("传入参数为空!");
		return NULL;
	}
	
	//LOGDEBUG("在%d端口创建消息通道成功!",port);
	while(!pThis->m_bExitTcpListenThread)
	{
		SSocket *pClient = new SSocket();
		if(!pThis->m_socket.Accept(*pClient))
		{
		
			LOGERROR("In ThreadCommandListen, 接受连接时失败!");
			delete pClient;
			if (pThis->m_bExitTcpListenThread)
			{
				break;
			}
			SApi::UsSleep(1000*1000);
			continue;
		}

		
		emit pThis->doAddHandle(pClient);
		/*stParamInfo stInfo;
		stInfo.m_pCurSocekt = pClient;
		stInfo.m_pPanelSimTcpServer = pThis;*/
		
#if 0
#ifdef WIN32
		HANDLE h = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ThreadSocketRecvService, (LPVOID)&stInfo, 0, 0);
		CloseHandle(h);
#else
		pthread_t t;
		pthread_create(&t, NULL, ThreadCommandService,(void*) &stInfo);
#endif
#endif
	}
	return NULL;
}
bool panel_sim_tcp_server::createServerSocket()
{
	if(!m_socket.Create())
	{
		LOGFAULT("In ThreadTcpListen Create failed! 创建消息通道失败!");
		return false;
	}

	int port = m_strPort.toInt();
	if(!m_socket.Bind(port))
	{
		LOGWARN("In ThreadTcpListen, 绑定端口%d到消息通道时失败!将尝试下一个端口",port);
		return false;
	}

	if(!m_socket.Listen())
	{
		LOGFAULT("In ThreadTcpListen, 侦听端口%d到消息通道时失败!",port);
		return false;
	}
	return true;
}

void panel_sim_tcp_server::OnSlotStop()
{
	//m_bExitHandleClientRecvThread = true;
	m_bExitTcpListenThread = true;
	m_socket.Close();
	ui.startPushButton->setEnabled(true);
	ui.stopPushButton->setEnabled(false);

	for(int i = ui.tabWidget->count()-1; i >= 0; i--)
	{
	
			ui.tabWidget->setCurrentIndex(i);
			panel_sim_tcp_recv_handle* pCur = (panel_sim_tcp_recv_handle*)ui.tabWidget->currentWidget();
			pCur->CloseSocket();
			//ui.tabWidget->removeTab(i);
	}
	
}

void panel_sim_tcp_server::OnSlotSocketClose(QString strText)
{
	for(int i = 0; i < ui.tabWidget->count(); i++)
	{
		if (ui.tabWidget->tabText(i) == strText)
		{
			ui.tabWidget->setCurrentIndex(i);
			ui.tabWidget->removeTab(i);
			return;
		}
	}
}

void panel_sim_tcp_server::OnSlotADD(SSocket* pCur)
{
	panel_sim_tcp_recv_handle* pNew = new panel_sim_tcp_recv_handle(this,pCur);
	ui.tabWidget->addTab(pNew,pCur->GetRemoteIpAddr().data());
	connect(pNew,SIGNAL(doSocketCloseHandle(QString )),this,SLOT(OnSlotSocketClose(QString )));
	m_recvPanelList.append(pNew);
}