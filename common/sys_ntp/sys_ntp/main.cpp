#include"SNtpApplication.h"

//#include"SNtp.h"
//void test()
//{
//	SNtpClient sn;
//	SDateTime time;
//	sn.SetServer("139.199.215.251 ",123);
//	bool b=sn.GetTime(time);
//}

SNtpApplication sntpApp;
int main(int argc, char* argv[])
{
	sntpApp.SetModuleDesc("ntp½ø³Ì");
	sntpApp.SetVersion("1.0.0");
	sntpApp.Run(argc,argv,__DATE__,__TIME__);
	return 0;
}