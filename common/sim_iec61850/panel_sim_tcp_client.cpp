#include "panel_sim_tcp_client.h"
#include <ctime>
#include <iostream>
static int g_iIndex = 0;
const char CCH[] = "_0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_";
panel_sim_tcp_client::panel_sim_tcp_client(QWidget *parent)
	: CBaseView(parent)
{
	ui.setupUi(this);
	init();
	initTable();
	m_bQuit = false;
	m_iSendBufSize = TCP_CLIENT_SEND_BUF;
	connect(ui.connectPushButton,SIGNAL(clicked()),this,SLOT(OnSlotConnect()));
	connect(ui.sendPushButton,SIGNAL(clicked()),this,SLOT(OnSlotSend()));
	connect(ui.unconnectPushButton,SIGNAL(clicked()),this,SLOT(OnSlotUnconnect()));
	connect(this,SIGNAL(doSocketUnconnect()),this,SLOT(OnSlotUnconnect()));
	connect(ui.randRadioButton,SIGNAL(clicked()),this,SLOT(OnSlotRandRadioButton()));
	connect(ui.hexRradioButton,SIGNAL(clicked()),this,SLOT(OnSlotHexRadioButton()));
	connect(ui.commonRradioButton,SIGNAL(clicked()),this,SLOT(OnSlotCommonRadioButton()));

	connect(ui.hexShowRadioButton,SIGNAL(clicked()),this,SLOT(OnSlotHexShowRadioButton()));
	connect(ui.stringShowRadioButton,SIGNAL(clicked()),this,SLOT(OnSlotStringShowRadioButton()));
}

panel_sim_tcp_client::~panel_sim_tcp_client()
{
	m_bQuit = true;
	m_socket.Close();
}


void panel_sim_tcp_client::init()
{
	QRegExp rx("((?:(?:25[0-5]|2[0-4]\\d|((1\\d{2})|([1-9]?\\d)))\\.){3}(?:25[0-5]|2[0-4]\\d|((1\\d{2})|([1-9]?\\d))))");
	QValidator *validatorIp=new QRegExpValidator(rx,ui.ipLineEdit);
	ui.ipLineEdit->setValidator(validatorIp);
	ui.ipLineEdit->setToolTip("请输入标准网络IP地址格式");
	ui.ipLineEdit->setText("127.0.0.1");

	QValidator *validatorPort=new QIntValidator(0,65535,ui.portLineEdit);
	ui.portLineEdit->setValidator(validatorPort);
	ui.portLineEdit->setToolTip("有效值范围0-65535");
	ui.unconnectPushButton->setEnabled(false);
	ui.connectPushButton->setEnabled(true);
	ui.portLineEdit->setText("7788");

	QValidator *validatorinterval=new QIntValidator(1,500000,ui.intervalLineEdit);
	ui.intervalLineEdit->setValidator(validatorinterval);
	ui.intervalLineEdit->setToolTip("有效值范围1-500000");
	ui.intervalLineEdit->setText("50");
	m_iIntervalValue = 50;

	QValidator *validatorLen=new QIntValidator(1,1024,ui.lenLineEdit);
	ui.lenLineEdit->setValidator(validatorLen);
	ui.lenLineEdit->setToolTip("有效值范围1-1024");
	ui.lenLineEdit->setText("32");
	m_iSendLen = 32;

	QValidator *validatorCount=new QIntValidator(1,1024,ui.sendNumLineEdit);
	ui.sendNumLineEdit->setValidator(validatorCount);
	ui.sendNumLineEdit->setToolTip("有效值范围1-1024");
	ui.sendNumLineEdit->setText("10");
	m_iSendCount = 10;
	ui.randRadioButton->setChecked(true);
	ui.hexRradioButton->setChecked(false);
	ui.commonRradioButton->setChecked(false);
	ui.stringShowRadioButton->setChecked(true);
}

void panel_sim_tcp_client::OnSlotConnect()
{
	QString strIp = ui.ipLineEdit->text();
	QString strPort = ui.portLineEdit->text();
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
	if (strPort.isEmpty())
	{
		QMessageBox::warning(this,"警告","服务器端口号不能为空！");
		return;
	}

	if(!m_socket.CreateTcp())
	{
		QMessageBox::warning(this,"警告","创建TCP的SOCKET失败！");
		return;
	}
	m_socket.SetTimeout(1000,1000);
	if(!m_socket.Connect(strIp.toStdString().data(), strPort.toInt()))
	{
		QMessageBox::warning(this,"警告","连接服务器失败！");
		return;
	}

	m_bQuit = false;
	ui.connectPushButton->setEnabled(false);
	ui.unconnectPushButton->setEnabled(true);
	SKT_CREATE_THREAD(ThreadTcpRecv,this);
}

void panel_sim_tcp_client::OnSlotSend()
{
	doCheckControlVaild();
	if (ui.randRadioButton->isChecked())
	{
		//char* pSendBuf = NULL;
		//pSendBuf = new char[m_iSendLen];
		getRandStr(m_sendRandomBuf,m_iSendLen);
		m_sendRandomBuf[m_iSendLen+1]='\0';
		m_pSendBuf = m_sendRandomBuf;
		m_iSendBufSize = m_iSendLen;
		//m_stSendMsgHead.iType = MSG_TYPE_STR;
		m_stSendMsgHead.iLen = m_iSendBufSize;
	} else if (ui.hexRradioButton->isChecked())
	{
		QString strHex = ui.sendInfoTextEdit->toPlainText();
		m_byteSendHexInfo = hexStringtoByteArray(strHex);
		m_pSendBuf = m_byteSendHexInfo.data();
		m_iSendBufSize = m_byteSendHexInfo.size();
		//m_stSendMsgHead.iType = MSG_TYPE_HEX;
		m_stSendMsgHead.iLen = m_iSendBufSize;
	} else 
	{
		m_strSendCommonInfo = ui.sendInfoTextEdit->toPlainText();
		m_pSendBuf = (char*)m_strSendCommonInfo.toStdString().data();
		m_iSendBufSize = m_strSendCommonInfo.size();
		//m_stSendMsgHead.iType = MSG_TYPE_STR;
		m_stSendMsgHead.iLen = m_iSendBufSize;
	}
#ifdef WIN32
	HANDLE h; 
	h = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)(int)ThreadTcpSend, (LPVOID)this, 0, 0); 
	::SetThreadPriority(h,THREAD_PRIORITY_HIGHEST);  
	CloseHandle(h);
#else
	SKT_CREATE_THREAD(ThreadTcpSend,this);
#endif
	//SKT_CREATE_THREAD(ThreadTcpSend,this);
}
bool panel_sim_tcp_client::doSend(char* pSendBuf, int len,int iType)
{
	stTCPMsgHead stHead;
	stHead.iLen = len;
	//stHead.iType = iType;
	for(int i = 0; i < m_iSendCount; i ++)
	{
		int iSendLen = m_socket.Send(&stHead,sizeof(stHead));
		if (iSendLen < 0)
		{
			return false;
		}
		iSendLen = m_socket.Send(pSendBuf,sizeof(len));
		if (iSendLen < 0)
		{
			return false;
		}
		SApi::UsSleep(m_iIntervalValue);
		
	}

	return true;
}
#if 1
void* panel_sim_tcp_client::ThreadTcpRecv(void *lp)
{
	panel_sim_tcp_client* pThis = (panel_sim_tcp_client*)lp;
	if (pThis == NULL)
	{
		emit pThis->doSocketUnconnect();
		return NULL;
	}
	char* pRecvBuf = new char[TCP_CLIENT_RECV_BUF];
	int iRecvBufSize = TCP_CLIENT_RECV_BUF;
	while(!pThis->IsQuit())
	{
		stTCPMsgHead stMsgHead={0};
		int iRecvLen = pThis->m_socket.Receive((void*)&stMsgHead,sizeof(stMsgHead));
		if (iRecvLen < 0)
		{
			if (pThis->IsQuit())
			{
				break;
			}
			QString strMsg = "接受失败，线程退出";
			pThis->addRow(strMsg);
			break;
		} else if (iRecvLen == 0)
		{
			continue;
		} 
		if (stMsgHead.iLen > iRecvBufSize)
		{
			delete [] pRecvBuf;
			pRecvBuf = NULL;
			pRecvBuf = new char[stMsgHead.iLen+1];
			iRecvBufSize = stMsgHead.iLen+1;
		}
		iRecvLen = pThis->m_socket.Receive(pRecvBuf,stMsgHead.iLen);
		if (iRecvLen < 0)
		{
			if (pThis->IsQuit())
			{
				break;
			}
			QString strMsg = "接受失败，线程退出";
			pThis->addRow(strMsg);
			break;
		} else if (iRecvLen == 0)
		{
			continue;
		} else
		{
			pThis->addRow(pRecvBuf,stMsgHead.iLen);
		}		
	}
	if (pRecvBuf)
	{
		delete[] pRecvBuf;
		pRecvBuf = NULL;
	}
	if (!pThis->IsQuit())
	{
		emit pThis->doSocketUnconnect();
	}
	
	return NULL;
}
#endif
#if 0
void* panel_sim_tcp_client::ThreadTcpRecv(void *lp)
{
	panel_sim_tcp_client* pThis = (panel_sim_tcp_client*)lp;
	if (pThis == NULL)
	{
		emit pThis->doSocketUnconnect();
		return NULL;
	}

	char chRecvBuf[TCP_CLIENT_RECV_BUF];
	while(!pThis->IsQuit())
	{
		int iRecvLen = pThis->m_socket.Receive(chRecvBuf,TCP_CLIENT_RECV_BUF-1);
		if (iRecvLen < 0)
		{
			pThis->m_bQuit = true;
			QString strMsg = "接受失败，线程退出";
			//pThis->ui.recvInfoTextEdit->setText(strMsg);
			pThis->addRow(strMsg);
			break;
		} else if (iRecvLen == 0)
		{
			continue;
		} else
		{
			chRecvBuf[iRecvLen +1]='\0';
			//pThis->addRow(strMsg);
			pThis->addRow(chRecvBuf,iRecvLen);
		}		
	}
	emit pThis->doSocketUnconnect();
	return NULL;
}
#endif
void panel_sim_tcp_client::doUnconnect()
{
	OnSlotUnconnect();
}

void panel_sim_tcp_client::OnSlotUnconnect()
{
	m_bQuit = true;
	m_socket.Close();
	ui.connectPushButton->setEnabled(true);
	ui.unconnectPushButton->setEnabled(false);
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
QByteArray panel_sim_tcp_client::hexStringtoByteArray(QString hex)
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
void panel_sim_tcp_client::formatString(QString &org, int n, const QChar &ch)
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

void panel_sim_tcp_client::getRandStr(char* pCH, int iLen)
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

void panel_sim_tcp_client::OnSlotRandRadioButton()
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
void panel_sim_tcp_client::OnSlotHexRadioButton()
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
void panel_sim_tcp_client::OnSlotCommonRadioButton()
{
	if (ui.commonRradioButton->isChecked())
	{
		ui.randRadioButton->setChecked(false);
		ui.hexRradioButton->setChecked(false);
	} else 
	{
		ui.randRadioButton->setChecked(false);
		ui.hexRradioButton->setChecked(false);
		ui.commonRradioButton->setChecked(true);
		ui.sendInfoTextEdit->clear();
	}
}

void panel_sim_tcp_client::initTable()
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

void panel_sim_tcp_client::addRow(QString& strText)
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

bool panel_sim_tcp_client::doCheckControlVaild()
{
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

void* panel_sim_tcp_client::ThreadTcpSend(void *lp)
{
	panel_sim_tcp_client* pThis = (panel_sim_tcp_client*)lp;
	if (pThis == NULL)
	{
		return NULL;
	}
	stTCPMsgHead stHead  = pThis->m_stSendMsgHead;
	for(int i = 0; i < pThis->m_iSendCount; i ++)
	{
		if (pThis->IsQuit())
		{
			break;
		}
#if 1
		int iSendLen = pThis->m_socket.Send(&stHead,sizeof(stHead));
		if (iSendLen < 0)
		{
			return false;
		}
#endif
		iSendLen = pThis->m_socket.Send(pThis->m_pSendBuf,stHead.iLen);
		if (iSendLen < 0)
		{
			return false;
		}
		SApi::UsSleep(pThis->m_iIntervalValue);
	}
	return NULL;
}

void panel_sim_tcp_client::OnSlotHexShowRadioButton()
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
void panel_sim_tcp_client::OnSlotStringShowRadioButton()
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

void panel_sim_tcp_client::addRow(char* pBuf, int len)
{
	int iRowCount = ui.tableWidget->rowCount();
	ui.tableWidget->insertRow(iRowCount);
	QString strCurTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
	QTableWidgetItem* item = new QTableWidgetItem(strCurTime);
	item->setTextAlignment(Qt::AlignCenter);
	item->setFlags(item->flags()&~Qt::ItemIsEditable);
	ui.tableWidget->setItem(iRowCount,0,item);

	QString strMsg;
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