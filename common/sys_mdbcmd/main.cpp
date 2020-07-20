// rms_mdbcmd.cpp : 定义控制台应用程序的入口点。
//

#include "MdbCmd.h"
#include <string.h>
#define APP_VER "2.0.1"

char g_ver[32];

int main(int argc, char* argv[])
{
	memset(g_ver, 0, sizeof(g_ver));
#ifndef WIN32
	strcpy(g_ver,APP_VER);
#else
	strcpy_s(g_ver, sizeof(g_ver), APP_VER);
#endif

	SLog::stopLogToConsole();
	SLog::setLogLevel(SLog::LOG_DEBUG);
	CMdbCmd cmd;
	cmd.Start();
//	while (1)
//	{
//		SApi::UsSleep(1000000);
//	}
	return 0;
}

