// test_mms_clt.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "SMmsClient.h"
//#include "SMmsServer.h"
#include "SLog.h"
#include "vld.h"

// class CMyMmsSvr : public SMmsServer
// {
// public:
// 	CMyMmsSvr(){};
// 	virtual ~CMyMmsSvr(){};
// 
// };

class CMyMms : public SMmsClient
{
public:
	CMyMms()
	{
	
	};
	virtual ~CMyMms()
	{

	}
	virtual void OnMmsReportProcess(stuSMmsReport* pMmsRpt)
	{
		printf("RPTID=%s\n",pMmsRpt->sRptID.data());
		for(int i=0;i<pMmsRpt->Items.count();i++)
		{
			printf("%d : ref=%s, val=%s, q=%s\n",i+1, GetMvlVarText(pMmsRpt->Items[i]->dataRefName).data()
				,GetMvlVarText(pMmsRpt->Items[i]->dataValue).replace("\n","").data()
				,GetMvlVarText(pMmsRpt->Items[i]->Reason).data());
		}
	};
};

void test_mmssvr()
{
// 	CMyMmsSvr svr;
// 	CMyMmsSvr::InitGlobal();
// 	svr.Start();
// 	printf("printf any key to quit!");
// 	getchar();
// 	CMyMmsSvr::ExitGlobal();
}
int main(int argc, _TCHAR* argv[])
{
	SLog::startLogToConsole();
	SLog::setLogLevel(SLog::LOG_DEBUG);
	LOGDEBUG("Into main");
	CMyMms mms;
	SLog::startLogToConsole();
	SLog::setLogLevel(SLog::LOG_DEBUG);
	mms.SetMmsServerAddr("10.145.58.210","",11001);
	SMmsClient::InitGlobal();
// 	mms.AddReportInfo("PT2201ALD0","LLN0$BR$brcbAlarm",1,5000,0x04);//PROT_LLN0_brcbdsTripInfo
// 	mms.AddReportInfo("PT2201APROT","LLN0$BR$brcbRelayDin",1,5000,0x04);//PROT_LLN0_brcbdsTripInfo
//	mms.AddReportInfo("PCS931LD0","LLN0$BR$brcbAlarm",1,5000,C_SMMS_TRGOPT_DATA);//PROT_LLN0_brcbdsTripInfo
//	mms.AddReportInfo("PCS931LD0","LLN0$BR$brcbRelaybDin",1,5000,C_SMMS_TRGOPT_DATA);//PROT_LLN0_brcbdsTripInfo
	mms.Start();

	while(1)
	{
		/*
		SPtrList<MMSValue> lstValue;
		bool ret = mms.ReadValue("ipacs5773_13LD0","RSYN1$SP$DifV$setMag",lstValue);
		printf("ReadValue ret=%d, cnt=%d\n",ret,lstValue.count());
		unsigned long pos;
		MMSValue *pV = lstValue.FetchFirst(pos);
		while(pV)
		{
			printf("%s=%s\n",pV->getName().data(),pV->getStrValue().data());
			pV = lstValue.FetchNext(pos);
		}
		SApi::UsSleep(1000000);
		continue;;
		*/
		printf("===============================================\n");
		printf("1 : GetMvlDirectory \n");
		printf("2 : GetMvlDirectory & sort \n");
		char ch = getchar();
		switch(ch)
		{
		case '1':
			{
				SPtrList<stuSMmsFileInfo> slFiles;
				unsigned long pos;
				int cnt = 0;
				stuSMmsFileInfo *p;
				mms.GetMvlDirectory("*",slFiles,false);
				p = slFiles.FetchFirst(pos);
				while(p)
				{
					printf("%d : %s,%d,%d,%s\n",++cnt,p->filename.data(),p->fsize,p->mtimpres,
						SDateTime::makeDateTime(p->mtime).toString("yyyy-MM-dd hh:mm:ss").data());
					p = slFiles.FetchNext(pos);
				}				
			}
			break;
		case '2':
			{
				SPtrList<stuSMmsFileInfo> slFiles;
				unsigned long pos;
				int cnt = 0;
				stuSMmsFileInfo *p;
				SString path = "/historyFile/中山_110kV九州站_1#变低5501/20171118/";
				path = path.toUtf8();
				mms.GetMvlDirectory(path.data(),slFiles,true);
				p = slFiles.FetchFirst(pos);
				while(p)
				{
					SString str = p->filename;
					str = str.toGb2312();
					printf("%d : %s\t%d\t%d\t%s\n",++cnt,str.data(),p->fsize,p->mtimpres,
						SDateTime::makeDateTime(p->mtime).toString("yyyy-MM-dd hh:mm:ss").data());
					p = slFiles.FetchNext(pos);
				}				
			}
			break;
		}
		if(ch == 'q')
			break;
	}
	

	printf("press any key to disconnect!\n");
	getchar();

	mms.Stop();

	SApi::UsSleep(1000000);
	SLog::WaitForLogEmpty();
	SLog::quitLog();
	SMmsClient::ExitGlobal();
	printf("press any key to quit!\n");
	getchar();
	return 0;
}

