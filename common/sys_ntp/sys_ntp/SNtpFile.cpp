#include "SNtpFile.h"


SNtpFile::SNtpFile()
{
	m_SntpClient = new SNtpClient();
	m_SntpServer = new SNtpServer();
}

SNtpFile::~SNtpFile()
{
	delete m_SntpClient;
	delete m_SntpServer;
}

bool SNtpFile::Load(SString sPathFile)
{
	//bool a=CConfigBase::Load(sPathFile);
	SXmlConfig xml;
	if(!xml.ReadConfig(sPathFile))
	{
		LOGFAULT("打开配置文件[%s]时失败!请检查文件是否存在且格式是否正确!\n",sPathFile.data());
		exit(0);
		return false;
	}
	LOGDEBUG("开始读取配置!\n");

	m_Lock.lock();
	SBaseConfig *pHd = xml.SearchChild("server");
	m_SntpServer->SetServer(pHd->GetAttribute("ip"), pHd->GetAttributeI("port"));
	if(pHd != NULL && pHd->GetAttribute("open").toLower() == "true")
		m_bOpenClient = true;
	else
		m_bOpenClient = false;
	pHd = xml.SearchChild("client");
	m_SntpClient->SetServer(pHd->GetAttribute("ip"), pHd->GetAttributeI("port"));
	if(pHd != NULL && pHd->GetAttribute("open").toLower() == "true")
		m_bOpenServer = true;
	else
		m_bOpenServer = false;
	m_iTimeSecs = pHd->GetAttributeI("m_iTimeSecs");
	
	LOGDEBUG("读取配置文件成功\n");

	m_Lock.unlock();
	return true;
}

bool SNtpFile::Run(int argc, char* argv[])
{
	if(this->m_bOpenClient)
	{
		if(this->m_SntpServer->StartServer(false))
		{
			LOGDEBUG("启动ntp服务端成功！\n");
		}
		else
		{
			LOGERROR("启动ntp服务端失败！请关闭程序重新启动！\n");
			return false;
		}
	}
	S_CREATE_THREAD(ThreadStart,this);
	return true;
}



void* SNtpFile::ThreadStart(void *lp)
{
	SNtpFile *pThis = (SNtpFile*)lp;
	while(pThis->m_bOpenServer)
	{
		if(!pThis->m_SntpClient->GetTime(pThis->m_Now))
			LOGERROR("获取当前服务器上时间失败！\n");
		for(int i=0;i<1000000;++i)
			SApi::UsSleep(pThis->m_iTimeSecs);//windows平台智能精确到1000微秒，参数为1微秒时会休眠1000微秒
	}
	return NULL;
}