/**
 *
 * 文 件 名 : main.cpp
 * 创建日期 : 2018-10-12 16:36
 * 作    者 : 邵凯田(skt001@163.com)
 * 修改日期 : $Date: $
 * 当前版本 : $Revision: $
 * 功能描述 : 桥接客户主程序
 * 修改记录 : 
 *            $Log: $
 *
 * Ver  Date        Author  Comments
 * ---  ----------  ------  -------------------------------------------
 * 001	2018-10-12	邵凯田　创建文件
 *
 **/

#include "BridgeClientApp.h"
#define MOD_VERSION "1.0.0"

#ifndef MOD_DATE
#define MOD_DATE "2018-10-12"
#endif
#ifndef MOD_TIME
#define MOD_TIME "22:06:00"
#endif

int main(int argc, char* argv[])
{
	CBridgeClientApp g_app;
	g_app.SetModuleDesc("桥接客户主程序");
	g_app.SetVersion(MOD_VERSION);
	//g_app.SetFork(false);
	g_app.Run(argc,argv,MOD_DATE,MOD_TIME);

	g_app.Stop();
	return 0;
}
