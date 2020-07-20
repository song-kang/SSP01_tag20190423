#include "panel_ntp_client.h"

panel_ntp_client::panel_ntp_client(QWidget *parent,int iType)
	: CBaseView(parent)
{
	ui.setupUi(this);
	m_strCurSetServerIp = "";
	m_strCurSetServerPort = "123";
	m_strCurSetClientIp = "182.92.12.11";
	m_strCurSetClientPort = "123";
	m_iNtpType = iType;
	init();
	refreshInfo();

	connect(ui.timeSynPushButton,SIGNAL(clicked()),this,SLOT(OnSlotTimeSyn()));
	connect(ui.startPushButton,SIGNAL(clicked()),this,SLOT(OnSlotStartServer()));
	connect(ui.stopPushButton,SIGNAL(clicked()),this,SLOT(OnSlotStopServer()));
	ui.timeSynPushButton->setToolTip("需要管理员权限运行程序");
}

panel_ntp_client::~panel_ntp_client()
{

}


void panel_ntp_client::init()
{
	QRegExp rx("((?:(?:25[0-5]|2[0-4]\\d|((1\\d{2})|([1-9]?\\d)))\\.){3}(?:25[0-5]|2[0-4]\\d|((1\\d{2})|([1-9]?\\d))))");
	QValidator *validatorIp=new QRegExpValidator(rx,ui.ipLineEdit);
	ui.ipLineEdit->setValidator(validatorIp);
	ui.ipLineEdit->setToolTip("请输入标准网络IP地址格式");
	

	QValidator *validatorPort=new QIntValidator(0,65535,ui.portLineEdit);
	ui.portLineEdit->setValidator(validatorPort);
	ui.portLineEdit->setToolTip("有效值范围0-65535");
}
void panel_ntp_client::refreshInfo()
{
	if (m_iNtpType == NTP_CLIENT)
	{
		ui.label->show();
		ui.ipLineEdit->show();
		ui.startPushButton->hide();
		ui.stopPushButton->hide();
		ui.timeSynPushButton->show();
		ui.ipLineEdit->setText(m_strCurSetClientIp);
		ui.portLineEdit->setText(m_strCurSetClientPort);
	} else
	{
		ui.label->hide();
		ui.ipLineEdit->hide();
		ui.startPushButton->show();
		ui.stopPushButton->show();
		ui.timeSynPushButton->hide();
		ui.ipLineEdit->setText(m_strCurSetServerIp);
		ui.portLineEdit->setText(m_strCurSetServerPort);
		ui.startPushButton->setEnabled(true);
		ui.stopPushButton->setEnabled(false);
	}
}
void panel_ntp_client::OnSlotTimeSyn()
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
	m_strCurSetClientIp = strIp;
	m_strCurSetClientPort = strPort;
	//182.92.12.11 可用时间
	m_ntpClent.SetServer(strIp.toStdString().data(),strPort.toInt());
	SDateTime strNowTime;
	if (m_ntpClent.GetTime(strNowTime))
	{
		SString strTime = strNowTime.toString("yyyy-MM-dd hh:mm:ss");
	} else 
	{
		QMessageBox::warning(this,"警告",tr("获取服务器[%1]时间失败！").arg(strIp));
		return;
	}

}

void panel_ntp_client::OnSlotStartServer()
{
	QString strIp = ui.ipLineEdit->text();
	QString strPort = ui.portLineEdit->text();
	if (!strIp.isEmpty())
	{
		QRegExp rx("((?:(?:25[0-5]|2[0-4]\\d|((1\\d{2})|([1-9]?\\d)))\\.){3}(?:25[0-5]|2[0-4]\\d|((1\\d{2})|([1-9]?\\d))))");
		bool bIsMatch = rx.exactMatch(strIp);
		if (!bIsMatch)
		{
			QMessageBox::warning(this,"警告","服务器IP地址不正确，请检查IP！");
			return;
		}
	}
	
	if (strPort.isEmpty())
	{
		QMessageBox::warning(this,"警告","服务器端口号不能为空！");
		return;
	}
	m_strCurSetServerIp = strIp;
	m_strCurSetServerPort = strPort;
	m_ntpServer.SetServer(strIp.toStdString().data(),strPort.toInt());
	if (m_ntpServer.StartServer())
	{
		ui.startPushButton->setEnabled(false);
		ui.stopPushButton->setEnabled(true);
	} else 
	{
		QMessageBox::warning(this,"警告",tr("获取NTP服务器失败！"));
		return;
	}
}

void panel_ntp_client::OnSlotStopServer()
{
	if (m_ntpServer.StopServer())
	{

	} else 
	{
		QMessageBox::warning(this,"警告",tr("停止NTP服务器失败！"));
		return;
	}
	ui.startPushButton->setEnabled(true);
	ui.stopPushButton->setEnabled(false);
}

void panel_ntp_client::OnRefresh()
{
	refreshInfo();
}