#include "panel_sim_net_storm.h"
const char CCH[] = "_0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_";
panel_sim_net_storm::panel_sim_net_storm(QWidget *parent)
	: CBaseView(parent)
{
	ui.setupUi(this);
	m_bThreadQuit = false;
	init();
	connect(ui.startPushButton,SIGNAL(clicked()),this,SLOT(OnSlotStart()));
	connect(ui.stopPushButton,SIGNAL(clicked()),this,SLOT(OnSlotStop()));
	connect(this,SIGNAL(doExistThread()),this,SLOT(OnExistThread()));
}

panel_sim_net_storm::~panel_sim_net_storm()
{
	m_bThreadQuit = true;
	Stop();
}

void panel_sim_net_storm::init()
{
	QValidator *validatorinterval=new QIntValidator(0,500000,ui.intervalLineEdit);
	ui.intervalLineEdit->setValidator(validatorinterval);
	ui.intervalLineEdit->setToolTip("有效值范围0-500000");
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
	ui.startPushButton->setChecked(true);
	ui.stopPushButton->setChecked(false);
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
QByteArray panel_sim_net_storm::hexStringtoByteArray(QString hex)
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
void panel_sim_net_storm::formatString(QString &org, int n, const QChar &ch)
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

void panel_sim_net_storm::getRandStr(char* pCH, int iLen)
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

void* panel_sim_net_storm::ThreadSend(void *lp)
{
	panel_sim_net_storm* pThis = (panel_sim_net_storm*)lp;
	int soc1,usec1;
	int usecs;
	pThis->BeginThread();

	unsigned long pos;
	while(!pThis->IsQuit())
	{
		if (pThis->m_iSendCount == 0)
		{
			pThis->m_Sender.SendRaw((BYTE*)pThis->m_pSendBuf,pThis->m_iSendBufSize);
		} else
		{
			for(int i = 0; i < pThis->m_iSendCount; i ++)
			{
				if (pThis->IsQuit())
				{
					break;
				}
				int irst = pThis->m_Sender.SendRaw((BYTE*)pThis->m_pSendBuf,pThis->m_iSendBufSize);
				SApi::UsSleep(pThis->m_iIntervalValue);
			}
			break;
		}
		
		SApi::UsSleep(pThis->m_iIntervalValue);
	}
	pThis->EndThread();
	if (!pThis->IsQuit())
	{
		emit pThis->doExistThread();
	}
	return NULL;
}

bool panel_sim_net_storm::doCheckControlVaild()
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
		m_iSendCount = 0;
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

void panel_sim_net_storm::OnSlotStart()
{
	if(!doCheckControlVaild())
	{
		return;
	}
	if (ui.randRadioButton->isChecked())
	{
		getRandStr(m_sendRandomBuf,m_iSendLen);
		m_sendRandomBuf[m_iSendLen+1]='\0';
		m_pSendBuf = m_sendRandomBuf;
		m_iSendBufSize = m_iSendLen;
	
	} else if (ui.hexRradioButton->isChecked())
	{
		QString strHex = ui.sendInfoTextEdit->toPlainText();
		m_byteSendHexInfo = hexStringtoByteArray(strHex);
		m_pSendBuf = m_byteSendHexInfo.data();
		m_iSendBufSize = m_byteSendHexInfo.size();
	} else 
	{
		m_strSendCommonInfo = ui.sendInfoTextEdit->toPlainText();
		m_pSendBuf = (char*)m_strSendCommonInfo.toStdString().data();
		m_iSendBufSize = m_strSendCommonInfo.size();
	}
	m_bThreadQuit = false;
	Start();
	ui.startPushButton->setEnabled(false);
	ui.stopPushButton->setEnabled(true);
}
void panel_sim_net_storm::OnSlotStop()
{
	m_bThreadQuit = true;
	OnExistThread();
}

////////////////////////////////////////////////////////////////////////
// 描    述:  启动服务,虚函数，派生函数中必须先调用此函数
// 作    者:  邵凯田
// 创建时间:  2016-01-07 09:57
// 参数说明:  void
// 返 回 值:  true表示服务启动成功,false表示服务启动失败
//////////////////////////////////////////////////////////////////////////
bool panel_sim_net_storm::Start()
{
	if(!SService::Start())
		return false;
	m_Sender.Close();
	if(!m_Sender.Create(m_sDevName))
	{
		LOGERROR("在设备[%s]上创建原始发送套接字失败!",m_sDevName.data());
		return false;
	}
#ifdef WIN32
	HANDLE h; 
	h = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)(int)ThreadSend, (LPVOID)this, 0, 0); 
	::SetThreadPriority(h,THREAD_PRIORITY_HIGHEST);  
	CloseHandle(h);
#else
	SKT_CREATE_THREAD(ThreadSend,this);
#endif
	return true;
}

////////////////////////////////////////////////////////////////////////
// 描    述:  停止服务，派生函数中必须先调用此函数
// 作    者:  邵凯田
// 创建时间:  2016-01-07 09:57
// 参数说明:  void
// 返 回 值:  true表示服务停止成功,false表示服务停止失败
//////////////////////////////////////////////////////////////////////////
bool panel_sim_net_storm::Stop()
{
	m_bThreadQuit = true;
	SService::Stop();
	//SService::StopAndWait();
	m_Sender.Close();
	return true;
}

void panel_sim_net_storm::OnRefresh()
{

	if (m_pCurTreeItem == NULL)
	{
		return;
	}
	if (m_pCurTreeItem == m_pOldTreeItem)
	{
		return;
	}

	m_bThreadQuit = true;
	QString qstr = m_pCurTreeItem->data(0,Qt::UserRole+1).toString();
	m_pCurSubnetwork = (stuSimSubnetwork*)atol(qstr.toStdString().data());
	if (m_pCurSubnetwork == NULL)
	{
		QMessageBox::warning(this,"警告","子网对象指针为空");
		return;
	}
	m_sDevName = m_pCurSubnetwork->name;
	m_pOldTreeItem = m_pCurTreeItem;
	ui.startPushButton->setEnabled(true);
	ui.stopPushButton->setEnabled(false);
}

void panel_sim_net_storm::OnExistThread()
{
	m_bThreadQuit = true;
	Stop();
	ui.startPushButton->setEnabled(true);
	ui.stopPushButton->setEnabled(false);
}