#include "panel_sim_udp.h"
#include <ctime>
#include <iostream>
const char CCH[] = "_0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_";
panel_sim_udp::panel_sim_udp(QWidget *parent)
	: CBaseView(parent)
{
	ui.setupUi(this);
	init();
	initTable();
	m_bQuit = false;
	m_iSendBufSize = 1024;
	m_bIsSocketStart = false;
	//ui.recvInfoTextEdit->setReadOnly(true);
	connect(ui.startPushButton,SIGNAL(clicked()),this,SLOT(OnSlotStart()));
	connect(ui.sendPushButton,SIGNAL(clicked()),this,SLOT(OnSlotSend()));
	connect(ui.stopPushButton,SIGNAL(clicked()),this,SLOT(OnSlotStop()));
	//connect(this,SIGNAL(doSocketUnconnect()),this,SLOT(OnSlotUnconnect()));
	connect(ui.randRadioButton,SIGNAL(clicked()),this,SLOT(OnSlotRandRadioButton()));
	connect(ui.hexRradioButton,SIGNAL(clicked()),this,SLOT(OnSlotHexRadioButton()));
	connect(ui.commonRradioButton,SIGNAL(clicked()),this,SLOT(OnSlotCommonRadioButton()));

	connect(ui.hexShowRadioButton,SIGNAL(clicked()),this,SLOT(OnSlotHexShowRadioButton()));
	connect(ui.stringShowRadioButton,SIGNAL(clicked()),this,SLOT(OnSlotStringShowRadioButton()));

	connect(this,SIGNAL(doExitRecvThread()),this,SLOT(OnSlotExitRecvThread()));
	
}

panel_sim_udp::~panel_sim_udp()
{
	m_bQuit = true;
	m_udpsocket.Close();
}


void panel_sim_udp::init()
{
	QRegExp rx("((?:(?:25[0-5]|2[0-4]\\d|((1\\d{2})|([1-9]?\\d)))\\.){3}(?:25[0-5]|2[0-4]\\d|((1\\d{2})|([1-9]?\\d))))");
	QValidator *validatorIp=new QRegExpValidator(rx,ui.ipLineEdit);
	ui.ipLineEdit->setValidator(validatorIp);
	ui.ipLineEdit->setToolTip("请输入标准网络IP地址格式");
	ui.ipLineEdit->setText("127.0.0.1");

	QValidator *validatorPort=new QIntValidator(0,65535,ui.portLineEdit);
	ui.portLineEdit->setValidator(validatorPort);
	ui.portLineEdit->setToolTip("有效值范围0-65535");
	ui.stopPushButton->setEnabled(false);
	ui.startPushButton->setEnabled(true);
	ui.portLineEdit->setText("7788");

	QValidator *validatorinterval=new QIntValidator(0,5000000,ui.intervalLineEdit);
	ui.intervalLineEdit->setValidator(validatorinterval);
	ui.intervalLineEdit->setToolTip("有效值范围0-5000000");
	ui.intervalLineEdit->setText("50");
	m_iIntervalValue = 50;

	QValidator *validatorLen=new QIntValidator(1,1024,ui.lenLineEdit);
	ui.lenLineEdit->setValidator(validatorLen);
	ui.lenLineEdit->setToolTip("有效值范围1-1024");
	ui.lenLineEdit->setText("32");
	m_iSendLen = 32;

	QValidator *validatorCount=new QIntValidator(1,10000,ui.sendNumLineEdit);
	ui.sendNumLineEdit->setValidator(validatorCount);
	ui.sendNumLineEdit->setToolTip("有效值范围1-10000");
	ui.sendNumLineEdit->setText("1");
	m_iSendCount = 1;
	ui.randRadioButton->setChecked(true);
	ui.hexRradioButton->setChecked(false);
	ui.commonRradioButton->setChecked(false);
	ui.stringShowRadioButton->setChecked(true);
	setSocketRecvSwitchState(true);
}

void panel_sim_udp::OnSlotStart()
{
	
	if(!doCheckControlVaild())
	{
		return;
	}
	if (!m_bIsSocketStart)
	{
		if(!CreateUdpSocket())
		{
			return;
		}
		if (!BindPort())
		{
			QMessageBox::warning(this,"警告","udp的socket绑定失败");
			return;
		}
	}
	m_bQuit = false;
	SKT_CREATE_THREAD(ThreadUdpRecv,this);
	setSocketRecvSwitchState(false);
}
void panel_sim_udp::setSocketRecvSwitchState(bool bState)
{
	ui.ipLineEdit->setEnabled(bState);
	ui.portLineEdit->setEnabled(bState);
	if (bState)
	{
		ui.startPushButton->setEnabled(true);
		ui.stopPushButton->setEnabled(false);
		m_bIsSocketStart = false;
	}else
	{
		ui.startPushButton->setEnabled(false);
		ui.stopPushButton->setEnabled(true);
		m_bIsSocketStart = true;
	}
}
void panel_sim_udp::OnSlotSend()
{
	if (!doCheckControlVaild())
	{
		return;
	}
	if (!m_bIsSocketStart)
	{
		if(!CreateUdpSocket())
		{
			return;
		}
	}

	if (ui.randRadioButton->isChecked())
	{
		memset(m_stUdpMsgHead.buf,0,sizeof(m_stUdpMsgHead.buf));
		getRandStr(m_stUdpMsgHead.buf,m_iSendLen);
		m_stUdpMsgHead.buf[m_iSendLen+1]='\0';
		m_chSendBuf = m_stUdpMsgHead.buf;
		m_iSendBufSize = m_iSendLen;
		//m_stUdpMsgHead.iType = MSG_TYPE_STR;
		m_stUdpMsgHead.iLen = m_iSendBufSize;
	} else if (ui.hexRradioButton->isChecked())
	{
		QString strHex = ui.sendInfoTextEdit->toPlainText();
		m_byteSendHexInfo = hexStringtoByteArray(strHex);
		int ilen = m_byteSendHexInfo.size();
		if (ilen > 1024)
		{
			ilen = 1024;
		}
		memcpy(m_stUdpMsgHead.buf,m_byteSendHexInfo.data(),ilen);
		m_stUdpMsgHead.buf[ilen+1]='\0';
		m_chSendBuf = m_stUdpMsgHead.buf;
		m_iSendBufSize = ilen;
		//m_stUdpMsgHead.iType = MSG_TYPE_HEX;
		m_stUdpMsgHead.iLen = m_iSendBufSize;
	} else 
	{
		m_strSendCommonInfo = ui.sendInfoTextEdit->toPlainText();
		int ilen = m_strSendCommonInfo.size();
		if (ilen > 1024)
		{
			ilen = 1024;
		}
		memcpy(m_stUdpMsgHead.buf,(char*)m_strSendCommonInfo.toStdString().data(),ilen);
		m_chSendBuf = m_stUdpMsgHead.buf;
		m_iSendBufSize = ilen;
		//m_stUdpMsgHead.iType = MSG_TYPE_STR;
		m_stUdpMsgHead.iLen = m_iSendBufSize;
	}
	m_bQuit = false;
#ifdef WIN32
	HANDLE h; 
	h = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)(int)ThreadUdpSend, (LPVOID)this, 0, 0); 
	::SetThreadPriority(h,THREAD_PRIORITY_HIGHEST);  
	CloseHandle(h);
#else
	SKT_CREATE_THREAD(ThreadUdpSend,this);
#endif
	//SKT_CREATE_THREAD(ThreadUdpSend,this);
}

/*************************************************
Function: hexStringtoByteArray()
Description: 十六进制字串转化为十六进制编码
Calls: formatString()
Called By:
Input: hex->待转化的十六进制字串
Output: NULL
Return: QByteArray
Others: NULL
*************************************************/
QByteArray panel_sim_udp::hexStringtoByteArray(QString hex)
{
    QByteArray ret;
    hex=hex.trimmed();
    //formatString(hex,2,' ');
    QStringList sl=hex.split(" ");
    foreach(QString s,sl)
    {
        if(!s.isEmpty())
            ret.append((char)s.toInt(0,16)&0xFF);
    }
    return ret;
}

/*************************************************
Function: formatString()
Description: 将十六进制字串每字节中间加空格分隔
Calls:
Called By:hexStringtoByteArray()
Input: org->待处理的字串
       n->间隔数默认为2
       ch->分隔标志，在此取空格
Output: NULL
Return: void
Others: NULL
*************************************************/
void panel_sim_udp::formatString(QString &org, int n, const QChar &ch)
{
    int size= org.size();
    int space= qRound(size*1.0/n+0.5)-1;
    if(space<=0)
        return;
    for(int i=0,pos=n;i<space;++i,pos+=(n+1))
    {
        org.insert(pos,ch);
    }
}

void panel_sim_udp::getRandStr(char* pCH, int iLen)
{
	 srand((unsigned)time(NULL));
	for (int i = 0; i < iLen; ++i)
	{
		//int x = rand() % (sizeof(CCH) - 1); //这个方法不好, 因为许多随机数发生器的低位比特并不随机,
		//RAND MAX 在ANSI 里#define 在<stdlib.h>

		//RAND MAX 是个常数, 它告诉你C 库函数rand() 的固定范围。

		//不可以设RAND MAX 为其它的值, 也没有办法要求rand() 返回其它范围的值。

		int x = rand() / (RAND_MAX / (sizeof(CCH) - 1));

		pCH[i] = CCH[x];
	}
}

void panel_sim_udp::OnSlotRandRadioButton()
{
	if (ui.randRadioButton->isChecked())
	{
		ui.randRadioButton->setChecked(true);
		ui.hexRradioButton->setChecked(false);
		ui.commonRradioButton->setChecked(false);
		
	} else 
	{
		ui.randRadioButton->setChecked(true);
		ui.hexRradioButton->setChecked(false);
		ui.commonRradioButton->setChecked(false);
		ui.sendInfoTextEdit->clear();
	}
}
void panel_sim_udp::OnSlotHexRadioButton()
{
	if (ui.hexRradioButton->isChecked())
	{
		ui.randRadioButton->setChecked(false);
		ui.hexRradioButton->setChecked(true);
		ui.commonRradioButton->setChecked(false);
	} else 
	{
		ui.randRadioButton->setChecked(false);
		ui.hexRradioButton->setChecked(true);
		ui.commonRradioButton->setChecked(false);
		ui.sendInfoTextEdit->clear();
	}
}
void panel_sim_udp::OnSlotCommonRadioButton()
{
	if (ui.commonRradioButton->isChecked())
	{
		ui.randRadioButton->setChecked(false);
		ui.hexRradioButton->setChecked(false);
		//ui.commonRradioButton->setChecked(false);
	} else 
	{
		ui.randRadioButton->setChecked(false);
		ui.hexRradioButton->setChecked(false);
		ui.commonRradioButton->setChecked(true);
		ui.sendInfoTextEdit->clear();
	}
}

void panel_sim_udp::initTable()
{
	ui.tableWidget->clear();
	QStringList labels;
	ui.tableWidget->setColumnCount(2);
	labels << "时间" << "消息内容" ; 
	ui.tableWidget->setHorizontalHeaderLabels(labels);
	ui.tableWidget->setColumnWidth(0,150);
	ui.tableWidget->setColumnWidth(1,250);
	SQt::SetTableWidgetStyle(ui.tableWidget);
}

void panel_sim_udp::addRow(QString& strText)
{
	int iRowCount = ui.tableWidget->rowCount();
	ui.tableWidget->insertRow(iRowCount);
	QString strCurTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
	QTableWidgetItem* item = new QTableWidgetItem(strCurTime);
	item->setTextAlignment(Qt::AlignCenter);
	item->setFlags(item->flags()&~Qt::ItemIsEditable);
	ui.tableWidget->setItem(iRowCount,0,item);

	strText = strText.trimmed();
	item = new QTableWidgetItem(strText);
	item->setTextAlignment(Qt::AlignCenter);
	item->setFlags(item->flags()&~Qt::ItemIsEditable);
	ui.tableWidget->setItem(iRowCount,1,item);

	ui.tableWidget->resizeRowsToContents();
	ui.tableWidget->resizeColumnsToContents();
}

void panel_sim_udp::addRow(char* pBuf, int len ,struct sockaddr_in& remote_addr)
{
	int iRowCount = ui.tableWidget->rowCount();
	ui.tableWidget->insertRow(iRowCount);
	QString strCurTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
	QTableWidgetItem* item = new QTableWidgetItem(strCurTime);
	item->setTextAlignment(Qt::AlignCenter);
	item->setFlags(item->flags()&~Qt::ItemIsEditable);
	ui.tableWidget->setItem(iRowCount,0,item);

	QString strRemoteIp = inet_ntoa(remote_addr.sin_addr);
	QString strMsg = tr("[IP:%1]:").arg(strRemoteIp);
	if (ui.hexShowRadioButton->isChecked())
	{
		SString strTemp;
		for (int i = 0; i < len; i ++)
		{
			printf("%02x ",pBuf[i]);
			strTemp += SString::toFormat("%02x ",(unsigned char)pBuf[i]);
		}
		strMsg = strTemp.trim().data();
	} else
	{
		pBuf[len]='\0';
		strMsg =pBuf;
	}
	item = new QTableWidgetItem(strMsg);
	item->setTextAlignment(Qt::AlignCenter);
	item->setFlags(item->flags()&~Qt::ItemIsEditable);
	ui.tableWidget->setItem(iRowCount,1,item);
}
bool panel_sim_udp::doCheckControlVaild()
{
	QString strIp = ui.ipLineEdit->text();
	QString strPort = ui.portLineEdit->text();
	if (strIp.isEmpty())
	{
		QMessageBox::warning(this,"警告","服务器IP地址不能为空！");
		return false;
	}

	QRegExp rx("((?:(?:25[0-5]|2[0-4]\\d|((1\\d{2})|([1-9]?\\d)))\\.){3}(?:25[0-5]|2[0-4]\\d|((1\\d{2})|([1-9]?\\d))))");
	bool bIsMatch = rx.exactMatch(strIp);
	if (!bIsMatch)
	{
		QMessageBox::warning(this,"警告","服务器IP地址不正确，请检查IP！");
		return false;
	}
	if (strPort.isEmpty())
	{
		QMessageBox::warning(this,"警告","服务器端口号不能为空！");
		return false;
	}
	m_strDestIp = strIp;
	m_strPort = strPort;
	if (ui.lenLineEdit->text().isEmpty())
	{
		m_iSendLen = 10;
	} else 
	{
		m_iSendLen = ui.lenLineEdit->text().toInt();
	}
	
	if (ui.sendNumLineEdit->text().isEmpty())
	{
		m_iSendCount = 1;
	} else 
	{
		m_iSendCount = ui.sendNumLineEdit->text().toInt();
	}
	if (ui.intervalLineEdit->text().isEmpty())
	{
		m_iIntervalValue = 50*1000;
	} else
	{
		m_iIntervalValue = ui.intervalLineEdit->text().toInt()*1000;
	}
	return true;
}
bool panel_sim_udp::CreateUdpSocket()
{
	if (!m_udpsocket.IsConnected())
	{
		return true;
	}
	if(!m_udpsocket.CreateUdp())
	{
		QMessageBox::warning(this,"警告","创建UDP的SOCKET失败！");
		return false;
	}
	return m_udpsocket.SetTimeout();
	
}
bool panel_sim_udp::BindPort()
{
	return m_udpsocket.Bind(m_strPort.toInt(),NULL);
}
void panel_sim_udp::OnSlotStop()
{
	m_bQuit = true;
	m_udpsocket.Close();
	setSocketRecvSwitchState(true);
}

 void* panel_sim_udp::ThreadUdpSend(void *lp)
 {
	 panel_sim_udp* pThis = (panel_sim_udp*)lp;
	 if (pThis == NULL)
	 {
		 return NULL;
	 }

	 for(int i = 0; i < pThis->m_iSendCount; i ++)
	 {
		 if (pThis->IsQuit())
		 {
			 break;
		 }
		 int iLen = pThis->m_udpsocket.SendTo((char*)pThis->m_strDestIp.toStdString().data(),pThis->m_strPort.toInt(),&pThis->m_stUdpMsgHead,sizeof(pThis->m_stUdpMsgHead));
		 if (iLen < 0)
		 {
			 break;
		 }
		 
		 SApi::UsSleep(pThis->m_iIntervalValue);
	 }
	 return NULL;
 }

 void* panel_sim_udp::ThreadUdpRecv(void *lp)
 {
	 panel_sim_udp* pThis = (panel_sim_udp*)lp;
	 if (pThis == NULL)
	 {
		 return NULL;
	 }
	 while(!pThis->IsQuit())
	 {
		 struct sockaddr_in remote_addr; //客户端网络地址结构体 
		 stUdpMsgHead stMsgHead;
		 memset(&stMsgHead,0,sizeof(stMsgHead));
		 int iRst = pThis->m_udpsocket.RecvFrom(&stMsgHead,sizeof(stMsgHead),remote_addr);
		 if (iRst < 0)
		 {
			 break;
		 } else if (iRst == 0)
		 {
			 continue;
		 }
		
		 pThis->addRow(stMsgHead.buf,iRst,remote_addr);
		 SApi::UsSleep(pThis->m_iIntervalValue);
	 }
	 if (!pThis->IsQuit())
	 {
		 emit pThis->doExitRecvThread();
	 }
	 
	 return NULL;
 }

 void panel_sim_udp::OnSlotExitRecvThread()
 {
	 m_bQuit = true;
	 m_udpsocket.Close();
	 setSocketRecvSwitchState(true);
 }

 bool panel_sim_udp::doRepaly(struct sockaddr_in &remote_addr )
 {

	 if (!ui.isAutoCheckBox->isChecked())
	 {
		 return true;
	 }

	 doCheckControlVaild();
	 stUdpMsgHead stSendMsg;
	 if (ui.randRadioButton->isChecked())
	 {
		 memset(stSendMsg.buf,0,sizeof(stSendMsg.buf));
		 getRandStr(stSendMsg.buf,m_iSendLen);
		 stSendMsg.buf[m_iSendLen+1]='\0';
		 m_chSendBuf = stSendMsg.buf;
		 m_iSendBufSize = m_iSendLen;
		 stSendMsg.iLen = m_iSendBufSize;
	 } else if (ui.hexRradioButton->isChecked())
	 {
		 QString strHex = ui.sendInfoTextEdit->toPlainText();
		 m_byteSendHexInfo = hexStringtoByteArray(strHex);
		 int ilen = m_byteSendHexInfo.size();
		 if (ilen > TCP_CLIENT_SEND_BUF-1)
		 {
			 ilen = TCP_CLIENT_SEND_BUF-1;
		 }
		 memcpy(stSendMsg.buf,m_byteSendHexInfo.data(),ilen);
		 stSendMsg.buf[ilen+1]='\0';
		 m_chSendBuf = stSendMsg.buf;
		 m_iSendBufSize = ilen;
		 stSendMsg.iLen = m_iSendBufSize;
	 } else 
	 {
		 m_strSendCommonInfo = ui.sendInfoTextEdit->toPlainText();
		 int ilen = m_strSendCommonInfo.size();
		 if (ilen > TCP_CLIENT_SEND_BUF - 1)
		 {
			 ilen = TCP_CLIENT_SEND_BUF - 1;
		 }
		 memcpy(stSendMsg.buf,(char*)m_strSendCommonInfo.toStdString().data(),ilen);
		 m_chSendBuf = stSendMsg.buf;
		 m_iSendBufSize = ilen;
		 stSendMsg.iLen = m_iSendBufSize;
	 }

	 int iLen = m_udpsocket.SendTo(remote_addr,&stSendMsg,sizeof(stSendMsg));
	 if (iLen < 0)
	 {
		return false;
	 }
	 return true;
 }

 void panel_sim_udp::OnSlotHexShowRadioButton()
 {
	 if (ui.hexShowRadioButton->isChecked())
	 {
		 ui.hexShowRadioButton->setChecked(true);
		 ui.stringShowRadioButton->setChecked(false);
	 } else 
	 {
		 ui.hexShowRadioButton->setChecked(false);
		 ui.stringShowRadioButton->setChecked(true);
	 }
 }
 void panel_sim_udp::OnSlotStringShowRadioButton()
 {
	 if (ui.stringShowRadioButton->isChecked())
	 {
		 ui.hexShowRadioButton->setChecked(false);
		 ui.stringShowRadioButton->setChecked(true);
	 } else 
	 {
		 ui.hexShowRadioButton->setChecked(false);
		 ui.stringShowRadioButton->setChecked(true);
	 }
 }
