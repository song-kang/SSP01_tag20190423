/**
 *
 * 文 件 名 : main.cpp
 * 创建日期 : 2015-8-1 13:31
 * 作    者 : 邵凯田(skt001@163.com)
 * 修改日期 : $Date: $
 * 当前版本 : $Revision: $
 * 功能描述 : 看门狗主程序(含硬件看门狗)
 * 修改记录 : 
 *            $Log: $
 *
 * Ver  Date        Author  Comments
 * ---  ----------  ------  -------------------------------------------
 * 001	2015-8-1	邵凯田　创建文件
 *
 **/

//////////////////////////////////////////////////////////////////////////
// History
// 1.0.0	2015-08-01  邵凯田　看门狗进程
//////////////////////////////////////////////////////////////////////////

#include "WatchApplication.h"

CWatchApplication g_app;

void test()
{
	SString str;
	SStringList list;
	int p,pid,i,ps=SApi::GetSystemProcess(list);
	for(i=0;i<ps;i++)
	{
		str = *list[i];
		printf("%s\n",str.data());
	}
	printf("\n\n");
	getchar();
}

int main(int argc, char* argv[])
{
	//test();
	g_app.SetModuleDesc("看门狗进程");
	g_app.SetVersion("1.0.0");
	g_app.Run(argc,argv,__DATE__,__TIME__);

	return 0;
}
