#include "panel_sim_tcp_recv_handle.h"
#include <ctime>
#include <iostream>

const char CCH[] = "_0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_";

panel_sim_tcp_recv_handle::panel_sim_tcp_recv_handle(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
}
panel_sim_tcp_recv_handle::panel_sim_tcp_recv_handle(panel_sim_tcp_server* pTcpServer,SSocket* pCurSocket,QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	m_pCurSocket = pCurSocket;
	m_pParentPanel =  pTcpServer;
	m_strCurSocketIp = m_pCurSocket->GetRemoteIpAddr().data();
	m_bQuit = false;
	m_pSendBuf = NULL;
	initTable();
	init();
	m_pCurSocket->SetTimeout(1000,1000);
	connect(ui.unconnectPushButton,SIGNAL(clicked()),this,SLOT(OnSlotUnconnect()));
	connect(ui.sendPushButton,SIGNAL(clicked()),this,SLOT(OnSlotSend()));

	connect(ui.randRadioButton,SIGNAL(clicked()),this,SLOT(OnSlotRandRadioButton()));
	connect(ui.hexRradioButton,SIGNAL(clicked()),this,SLOT(OnSlotHexRadioButton()));
	connect(ui.commonRradioButton,SIGNAL(clicked()),this,SLOT(OnSlotCommonRadioButton()));

	connect(ui.hexShowRadioButton,SIGNAL(clicked()),this,SLOT(OnSlotHexShowRadioButton()));
	connect(ui.stringShowRadioButton,SIGNAL(clicked()),this,SLOT(OnSlotStringShowRadioButton()));
	SKT_CREATE_THREAD(ThreadRecv,this);
}
panel_sim_tcp_recv_handle::~panel_sim_tcp_recv_handle()
{
	m_bQuit = true;
	if (m_pCurSocket)
	{
		m_pCurSocket->Close();
		delete m_pCurSocket;
		m_pCurSocket = NULL;
	}
}

void panel_sim_tcp_recv_handle::init()
{
	ui.ipLineEdit->setText(m_pCurSocket->GetRemoteIpAddr().data());
	ui.portLineEdit->setText(QString::number(m_pCurSocket->GetRemotePort()));
	ui.ipLineEdit->setEnabled(false);
	ui.portLineEdit->setEnabled(false);

	QValidator *validatorinterval=new QIntValidator(1,500000,ui.intervalLineEdit);
	ui.intervalLineEdit->setValidator(validatorinterval);
	ui.intervalLineEdit->setToolTip("有效值范围1-500000");
	ui.intervalLineEdit->setText("50");
	m_iIntervalValue = 50;

	QValidator *validatorLen=new QIntValidator(1,1024,ui.lenLineEdit);
	ui.lenLineEdit->setValidator(validatorLen);
	ui.lenLineEdit->setToolTip("有效值范围1-1024");
	ui.lenLineEdit->setText("10");
	m_iSendLen = 10;

	QValidator *validatorCount=new QIntValidator(1,1024,ui.sendNumLineEdit);
	ui.sendNumLineEdit->setValidator(validatorCount);
	ui.sendNumLineEdit->setToolTip("有效值范围1-1024");
	ui.sendNumLineEdit->setText("1");
	m_iSendCount = 1;


	ui.randRadioButton->setChecked(true);
	ui.hexRradioButton->setChecked(false);
	ui.commonRradioButton->setChecked(false);

	ui.isAutoCheckBox->setChecked(false);
	ui.stringShowRadioButton->setChecked(true);
}

void panel_sim_tcp_recv_handle::initTable()
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

void panel_sim_tcp_recv_handle::addRow(QString& strText)
{
	int iRowCount = ui.tableWidget->rowCount();
	ui.tableWidget->insertRow(iRowCount);
	QString strCurTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
	QTableWidgetItem* item = new QTableWidgetItem(strCurTime);
	item->setTextAlignment(Qt::AlignCenter);
	item->setFlags(item->flags()&~Qt::ItemIsEditable);
	ui.tableWidget->setItem(iRowCount,0,item);

	item = new QTableWidgetItem(strText);
	item->setTextAlignment(Qt::AlignCenter);
	item->setFlags(item->flags()&~Qt::ItemIsEditable);
	ui.tableWidget->setItem(iRowCount,1,item);
}
void panel_sim_tcp_recv_handle::addRow(char* pBuf, int len)
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
QByteArray panel_sim_tcp_recv_handle::hexStringtoByteArray(QString hex)
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
void panel_sim_tcp_recv_handle::formatString(QString &org, int n, const QChar &ch)
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

void panel_sim_tcp_recv_handle::getRandStr(char* pCH, int iLen)
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

void* panel_sim_tcp_recv_handle::ThreadRecv(void *lp)
{
	panel_sim_tcp_recv_handle* pThis = (panel_sim_tcp_recv_handle*)lp;
	char* pRecvBuf = new char[TCP_CLIENT_RECV_BUF];
	if (pRecvBuf == NULL)
	{
		QMessageBox::warning(pThis,"警告","内存申请失败");
		return NULL;
	}
	int iRecvBufSize = TCP_CLIENT_RECV_BUF;
	while(!pThis->IsQuit())
	{
		//stSocketMsg msg={0};
		stTCPMsgHead stHead={0};
		int iRecvLen = pThis->m_pCurSocket->Receive(&stHead,sizeof(stHead));
		if (iRecvLen < 0)
		{
			break;
		} else if (iRecvLen == 0)
		{
			//QString strMsg = "接受超时";
			//pThis->ui.recvInfoTextEdit->setText(strMsg);
			//pThis->doUnconnect();
			//return NULL;
			continue;
		} 
		if (stHead.iLen > iRecvBufSize)
		{
			delete [] pRecvBuf;
			pRecvBuf = NULL;
			pRecvBuf = new char[stHead.iLen+1];
			iRecvBufSize = stHead.iLen+1;
		}
		iRecvLen = pThis->m_pCurSocket->Receive(pRecvBuf,stHead.iLen);
		if (iRecvLen < 0)
		{
			break;
		} else if (iRecvLen == 0)
		{
			//QString strMsg = "接受超时";
			//pThis->ui.recvInfoTextEdit->setText(strMsg);
			//pThis->doUnconnect();
			//return NULL;
			continue;
		} else
		{
			pThis->addRow(pRecvBuf,stHead.iLen);
			pThis->doRepaly();

		}
	}
	if (pRecvBuf)
	{
		delete[] pRecvBuf;
		pRecvBuf = NULL;
	}
	if (!pThis->IsQuit())
	{
		emit pThis->doSocketCloseHandle(pThis->m_strCurSocketIp);
	}
	
	return NULL;
}


 bool panel_sim_tcp_recv_handle::doRepaly()
 {
	 if (!ui.isAutoCheckBox->isChecked())
	 {
		 return true;
	 }

	 doCheckControlVaild();
	
	 if (ui.randRadioButton->isChecked())
	 {
		 char chRandBuf[TCP_CLIENT_SEND_BUF]={0};
		 getRandStr(chRandBuf,m_iSendLen);
		 chRandBuf[m_iSendLen+1]='\0';
		 return doSend(chRandBuf,m_iSendLen,MSG_TYPE_STR);
	 } else if (ui.hexRradioButton->isChecked())
	 {
		 QString strHex = ui.sendInfoTextEdit->toPlainText();
		 QByteArray byteHex = hexStringtoByteArray(strHex);
		 return doSend(byteHex.data(),byteHex.size(),MSG_TYPE_HEX);
	 } else 
	 {
		 QString strText = ui.sendInfoTextEdit->toPlainText();
		 return doSend((char*)strText.toStdString().c_str(),strText.size(),MSG_TYPE_STR);
	 }
	 return true;
 }
 bool panel_sim_tcp_recv_handle::doSend(char* pSendBuf, int len,int iType)
 {
	 stTCPMsgHead stHead;
	 stHead.iLen = len;
	 //stHead.iType = iType;
	
	 //for(int i = 0; i < m_iSendCount; i ++)
	 {
#if 1
		 int iSendLen = m_pCurSocket->Send(&stHead,sizeof(stHead));
		 if (iSendLen < 0)
		 {
			 return false;
		 }
#endif
		 iSendLen = m_pCurSocket->Send(pSendBuf,len);
		 if (iSendLen < 0)
		 {
			 return false;
		 }
		 //SApi::UsSleep(m_iIntervalValue);
		 //msleep(m_iIntervalValue);
	 }
	 return true;
 }
 bool panel_sim_tcp_recv_handle::doCheckControlVaild()
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

 void panel_sim_tcp_recv_handle::OnSlotSend()
 {
	 doCheckControlVaild();
	 //m_iIntervalValue = 0;
	 //m_iSendCount = 1;
	 if (ui.randRadioButton->isChecked())
	 {
		 getRandStr(m_sendRandBuf,m_iSendLen);
		 m_sendRandBuf[m_iSendLen+1]='\0';
		 m_stSendMsgHead.iLen = m_iSendLen;
		 //m_stSendMsgHead.iType = MSG_TYPE_STR;
		 m_pSendBuf = m_sendRandBuf;
		 m_iSendBufSize = m_iSendLen;
	 } else if (ui.hexRradioButton->isChecked())
	 {
		 QString strHex = ui.sendInfoTextEdit->toPlainText();
		 m_byteSendHexInfo = hexStringtoByteArray(strHex);
		 m_stSendMsgHead.iLen = m_iSendLen;
		 //m_stSendMsgHead.iType = MSG_TYPE_HEX;
		 m_pSendBuf = m_byteSendHexInfo.data();
		 m_iSendBufSize = m_byteSendHexInfo.size();
	 } else 
	 {
		 QString strText = ui.sendInfoTextEdit->toPlainText();
		 m_stSendMsgHead.iLen = m_iSendLen;
		 //m_stSendMsgHead.iType = MSG_TYPE_STR;
		 m_pSendBuf = (char*)m_strSendCommonInfo.toStdString().data();
		 m_iSendBufSize = m_strSendCommonInfo.size();
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

 void panel_sim_tcp_recv_handle::OnSlotUnconnect()
 {
	 m_bQuit = true;
	 m_pCurSocket->Close();
	 emit doSocketCloseHandle(m_strCurSocketIp);
 }

 void panel_sim_tcp_recv_handle::CloseSocket()
 {
	 OnSlotUnconnect();
 }

 void* panel_sim_tcp_recv_handle::ThreadTcpSend(void *lp)
 {
	 panel_sim_tcp_recv_handle* pThis = (panel_sim_tcp_recv_handle*)lp;
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
		 int iSendLen = pThis->m_pCurSocket->Send(&stHead,sizeof(stHead));
		 if (iSendLen < 0)
		 {
			 return false;
		 }
#endif
		 iSendLen = pThis->m_pCurSocket->Send(pThis->m_pSendBuf,stHead.iLen);
		 if (iSendLen < 0)
		 {
			 return false;
		 }
		 SApi::UsSleep(pThis->m_iIntervalValue);
	 }
	 return NULL;
 }

 void panel_sim_tcp_recv_handle::OnSlotHexShowRadioButton()
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
 void panel_sim_tcp_recv_handle::OnSlotStringShowRadioButton()
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

 void panel_sim_tcp_recv_handle::OnSlotRandRadioButton()
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
	 }
 }
 void panel_sim_tcp_recv_handle::OnSlotHexRadioButton()
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
	 }
 }
 void panel_sim_tcp_recv_handle::OnSlotCommonRadioButton()
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
	 }
 }