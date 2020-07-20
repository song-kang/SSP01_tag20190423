/**
 *
 * 文 件 名 : sp_log_config.cpp
 * 创建日期 : 2015-8-1 13:03
 * 作    者 : 邵凯田(skt001@163.com)
 * 修改日期 : $Date: $
 * 当前版本 : $Revision: $
 * 功能描述 : 日志配置文件操作类
 * 修改记录 : 
 *            $Log: $
 *
 * Ver  Date        Author  Comments
 * ---  ----------  ------  -------------------------------------------
 * 001	2015-8-1	邵凯田　创建文件
 *
 **/

#include "sp_log_config.h"
#include "SXmlConfig.h"

CLogConfig::CLogConfig()
{
	SetConfigType(SPCFG_LOG_CONFIG);
	SetAutoReload(true);//允许重新加载
}
CLogConfig::~CLogConfig()
{
	
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  加载配置文件
// 作    者:  邵凯田
// 创建时间:  2015-8-1 13:04
// 参数说明:  @sPathFile配置文件名
// 返 回 值:  true/false
//////////////////////////////////////////////////////////////////////////
bool CLogConfig::Load(SString sPathFile)
{
	SXmlConfig xml;
	if(!xml.ReadConfig(sPathFile))
	{
		LOGWARN("打开<sys_log.xml>文件失败!将无法使用日志记录功能! file=%s",sPathFile.data());
		return false;
	}
	SString sModule = SLog::getModuleName();
	SBaseConfig *pPrecommand = xml.SearchChild("log<module='"+sModule+"'>");
	if(pPrecommand == NULL)
	{
		//去除.exe再找一次
		if(sModule.right(4).toLower() == ".exe")
			sModule = sModule.left(sModule.length()-4);
		pPrecommand = xml.SearchChild("log<module='"+sModule+"'>");
	}
	if(pPrecommand == NULL)//没有专用的，找缺省的
		pPrecommand = xml.SearchChild("log<module=''>");
	if(pPrecommand == NULL)//没有专用的，找缺省的
		pPrecommand = xml.SearchChild("log<module='*'>");
	if(pPrecommand == NULL)
	{
		printf("没有找到合适的日志配置信息!读取日志配置失败!\n");
		return false;
	}
	//预处理命令
	/*
	<log level="debug">
		<concole open="true"/>
		<tcp open="true" ip="192.168.0.208" port="7001" />
		<file open="true" size="1024000" files="5" file="d:/sys/log/log_[@modulename].txt"/>
	*/
	SString level = pPrecommand->GetAttribute("level").toLower();
	if(level == "basedebug")
		SLog::setLogLevel(SLog::LOG_BASEDEBUG);
	else if(level == "debug")
		SLog::setLogLevel(SLog::LOG_DEBUG);
	else if(level == "warn")
		SLog::setLogLevel(SLog::LOG_WARN);
	else if(level == "error")
		SLog::setLogLevel(SLog::LOG_ERROR);
	else if(level == "fault")
		SLog::setLogLevel(SLog::LOG_FAULT);
	else if(level == "none" || level == "" || level == "null" || level == "no")
		SLog::setLogLevel(SLog::LOG_NONE);
	else
		SLog::setLogLevel(SLog::LOG_DEBUG);
	if(pPrecommand->SearchNodeAttribute("concole","open").toLower() == "true")
		SLog::startLogToConsole();
	else
		SLog::stopLogToConsole();
	if(pPrecommand->SearchNodeAttribute("tcp","open").toLower() == "true")
	{
		SString ip = pPrecommand->SearchNodeAttribute("tcp","ip");
		int port = pPrecommand->SearchNodeAttributeI("tcp","port");
		SLog::startLogToTCPSvr(ip.data(),port);
	}
	if(pPrecommand->SearchNodeAttribute("file","open").toLower() == "true")
	{
		SString file = pPrecommand->SearchNodeAttribute("file","file");
		file = file.replace("[@modulename]",sModule);
		int size = pPrecommand->SearchNodeAttributeI("file","size");
		int files = pPrecommand->SearchNodeAttributeI("file","files");
		if(files < 1)
		{
			printf("无效的日志文件数设置:%d，将强制设为1！\n",files);
			files = 1;
		}
		if(size < 1024)
		{
			printf("过小的日志文件大小设置:%d,将强制设为1024!\n",size);
			size = 1024;
		}
		if(size > 104857600)//100M
		{
			printf("过大的日志文件大小设置:%d,将强制设为104857600!",size);
			size = 104857600;
		}
		SLog::startLogToFile(file.data());
		SLog::setLogFileMaxSize(size);
		SLog::setLogFileMaxBackFiles(files);
	}
	return true;
}
