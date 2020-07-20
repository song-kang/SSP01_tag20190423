#include "SNtpApplication.h"

SNtpApplication::SNtpApplication()
{

}
SNtpApplication::~SNtpApplication()
{

}

//////////////////////////////////////////////////////////////////////////
// 描    述:  启动服务，开启所有应用的服务，该函数必须由派生类实现，且启动服务完成后必须返回
// 作    者:  邵凯田
// 创建时间:  2015-8-1 13:34
// 参数说明:  void
// 返 回 值:  true/false
//////////////////////////////////////////////////////////////////////////
bool SNtpApplication::Start()
{
	m_SntpFile.Load(GetConfPath()+"sys_ntp.xml");
	if(!m_SntpFile.Run(m_iArgc,m_ppArgv))
		Quit();//退出运行
	return true;
}

bool SNtpApplication::Stop()
{
	return true;
}

bool SNtpApplication::ProcessAgentMsg(WORD wMsgType,stuSpUnitAgentMsgHead *pMsgHead,SString &sHeadStr,BYTE* pBuffer/*=NULL*/,int iLength/*=0*/)
{
	return false;
}