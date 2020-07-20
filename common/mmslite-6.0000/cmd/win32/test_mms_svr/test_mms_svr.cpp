// test_mms_clt.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "SMmsServer.h"
#include "SLog.h"
//#include "vld.h"
  
class CMyMmsSvr : public SMmsServer
{
public:
	CMyMmsSvr()
	{
		m_map.setAutoDelete(true);
	};
	virtual ~CMyMmsSvr(){};

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  映射叶子节点用户
	// 作    者:  邵凯田
	// 创建时间:  2016-8-30 8:55
	// 参数说明:  @sIedLdName表示IED+LD名称
	//         :  @sLeafPath表示叶子节点路径
	//         :  @ppLeafMap表示叶子节点的用户映射指针的地址，映射指针为空，用户层决定是否需要映射，需要时创建stuLeafMap对象并填写指针，MMS服务内部不负责释放
	// 返 回 值:  true表示成功映射，false表示无法映射
	//////////////////////////////////////////////////////////////////////////
	virtual bool OnMapLeafToUser(char* sIedLdName, char* sLeafPath, stuLeafMap **ppLeafMap)
	{
		if(strstr(sLeafPath,"stVal") != NULL)
		{
			*ppLeafMap = new stuLeafMap();
			m_map.append(*ppLeafMap);
			return true;
		}
		return false;
	}
	//virtual bool OnReadLeafValue(const char* sIedLdName, const char* sLeafPath, stuLeafMap *pLeafMap, void *mvluRdVaCtrl, int reason = 0){return false;};
	SPtrList<stuLeafMap> m_map;
};

void test_mmssvr()
{

}

void* ThreadRpt(void* lp)
{
	CMyMmsSvr *pSvr = (CMyMmsSvr*)lp;
	unsigned long pos;
	int stVal = 0;
	while(1)
	{
		stVal++;
		if(stVal > 1)
			stVal = 0;
		stuLeafMap *pMap = pSvr->m_map.FetchFirst(pos);
		while(pMap)
		{
			pSvr->NewReportPush(pMap,stVal);	
			pMap = pSvr->m_map.FetchNext(pos);
		}
		pSvr->NewReportProc();
		SApi::UsSleep(3000000);
	}
}

int main(int argc, _TCHAR* argv[])
{
	SLog::startLogToConsole();
	SLog::setLogLevel(SLog::LOG_DEBUG);
	LOGDEBUG("Into main");
	SLog::startLogToConsole();
	SLog::setLogLevel(SLog::LOG_DEBUG);
	CMyMmsSvr svr;
	svr.SetRootPath("d:\\");
	//SString scd = "sac.scd";
	//SString scd = "110kV大侣变-20100319.scd";//"uk.scd";
	SString scd = "Z:\\Software_Platform\\common\\sim_iec61850\\bin\\rms.scd";//"uk.scd";
	scd="Z:\\Software_Platform\\common\\sim_iec61850\\bin\\uk.scd";
	scd="Z:\\Software_Platform\\common\\sim_iec61850\\conf\\1.scd";//Z:\Software_Platform\common\sim_iec61850\conf
	scd="d:\\uk.scd";
	//SString scd = "兰溪工程竣工版本111207.scd";//"uk.scd";
#if 1
	printf("begin to add by scd... \n");

	/*
	解释SCL 1 (file=Z:\Software_Platform\common\sim_iec61850\conf\1.scd,IED=1-PT2201,AP=S1)结果为:0, 用时:612.000000ms
	解释SCL 2 (file=Z:\Software_Platform\common\sim_iec61850\conf\1.scd,IED=2-CL2201,AP=S1)结果为:0, 用时:721.000000ms
	解释SCL 3 (file=Z:\Software_Platform\common\sim_iec61850\conf\1.scd,IED=3-CL2202,AP=S1)结果为:0, 用时:775.000000ms
	解释SCL 4 (file=Z:\Software_Platform\common\sim_iec61850\conf\1.scd,IED=4-PL2201,AP=S1)结果为:0, 用时:1035.000000ms
	解释SCL 5 (file=Z:\Software_Platform\common\sim_iec61850\conf\1.scd,IED=5-PL2202,AP=S1)结果为:0, 用时:1083.000000ms
	解释SCL 6 (file=Z:\Software_Platform\common\sim_iec61850\conf\1.scd,IED=6-PF2201,AP=S1)结果为:0, 用时:2201.000000ms
	解释SCL 7 (file=Z:\Software_Platform\common\sim_iec61850\conf\1.scd,IED=7-PM2201,AP=S1)结果为:0, 用时:1405.000000ms
	解释SCL 8 (file=Z:\Software_Platform\common\sim_iec61850\conf\1.scd,IED=8-PM2201,AP=G1)结果为:0, 用时:1174.000000ms
	解释SCL 9 (file=Z:\Software_Platform\common\sim_iec61850\conf\1.scd,IED=9-CT2201,AP=S1)结果为:0, 用时:1390.000000ms
	解释SCL 10 (file=Z:\Software_Platform\common\sim_iec61850\conf\1.scd,IED=10-PL1101,AP=S1)结果为:0, 用时:1738.000000ms
	解释SCL 11 (file=Z:\Software_Platform\common\sim_iec61850\conf\1.scd,IED=11-PL1102,AP=S1)结果为:0, 用时:1902.000000ms
	解释SCL 12 (file=Z:\Software_Platform\common\sim_iec61850\conf\1.scd,IED=12-PL1001,AP=S1)结果为:0, 用时:1257.000000ms
	解释SCL 13 (file=Z:\Software_Platform\common\sim_iec61850\conf\1.scd,IED=13-PL1002,AP=S1)结果为:0, 用时:1210.000000ms
	解释SCL 14 (file=Z:\Software_Platform\common\sim_iec61850\conf\1.scd,IED=14-PC1001,AP=S1)结果为:0, 用时:1133.000000ms
	解释SCL 15 (file=Z:\Software_Platform\common\sim_iec61850\conf\1.scd,IED=15-PC1002,AP=S1)结果为:0, 用时:1152.000000ms
	解释SCL 16 (file=Z:\Software_Platform\common\sim_iec61850\conf\1.scd,IED=16-PS1001,AP=S1)结果为:0, 用时:1013.000000ms
	解释SCL 17 (file=Z:\Software_Platform\common\sim_iec61850\conf\1.scd,IED=17-PS1002,AP=S1)结果为:0, 用时:963.000000ms
	解释SCL 18 (file=Z:\Software_Platform\common\sim_iec61850\conf\1.scd,IED=18-ECFL,AP=S1)结果为:0, 用时:898.000000ms
	解释SCL 19 (file=Z:\Software_Platform\common\sim_iec61850\conf\1.scd,IED=19-HJJC,AP=S1)结果为:0, 用时:931.000000ms
	解释SCL 20 (file=Z:\Software_Platform\common\sim_iec61850\conf\1.scd,IED=20-XDC,AP=S1)结果为:0, 用时:946.000000ms
	解释SCL 21 (file=Z:\Software_Platform\common\sim_iec61850\conf\1.scd,IED=21-YTHDY,AP=S1)结果为:0, 用时:895.000000ms
	解释SCL 22 (file=Z:\Software_Platform\common\sim_iec61850\conf\1.scd,IED=22-GHJ,AP=S1)结果为:3, 用时:403.000000ms
	IED:22-GHJ/S1模型加载失败!
	解释SCL 23 (file=Z:\Software_Platform\common\sim_iec61850\conf\1.scd,IED=23-GPS,AP=S1)结果为:3, 用时:269.000000ms
	IED:23-GPS/S1模型加载失败!

	*/
	svr.AddIedByScd(scd);
	/*
	svr.AddIed(stuMmsServerIed::New(scd,1,"PT2201","S1"));
 	svr.AddIed(stuMmsServerIed::New(scd,1,"GHJ","S1"));
 	svr.AddIed(stuMmsServerIed::New(scd,1,"GPS","S1"));
	*/
	printf("ieds=%d\n",svr.m_Ieds.count());
#else
	SXmlConfig xml;
	if(!xml.ReadConfig(scd))
	{
		printf("open scl file:%s error!\n",scd.data());
		return 0;
	}
	SBaseConfig *pCommunication = xml.SearchChild("Communication");
	if(pCommunication != NULL)
	{
		int cnt = 0;
		unsigned long pos,pos2;
		SBaseConfig *pSubNetwork = pCommunication->GetChildPtr()->FetchFirst(pos);
		while(pSubNetwork)
		{
			if(pSubNetwork->GetAttribute("type").find("MMS") >= 0)
			{
				SBaseConfig *pConnectedAP = pSubNetwork->GetChildPtr()->FetchFirst(pos2);
				while(pConnectedAP)
				{
					svr.AddIed(stuMmsServerIed::New(scd,0,pConnectedAP->GetAttribute("iedName"),pConnectedAP->GetAttribute("apName")));
					printf("cnt=%d %s %s\n",++cnt,pConnectedAP->GetAttribute("iedName").data(),pConnectedAP->GetAttribute("apName").data());
					pConnectedAP = pSubNetwork->GetChildPtr()->FetchNext(pos2);
				}
			}
			pSubNetwork = pCommunication->GetChildPtr()->FetchNext(pos);
		}
	}
	xml.clear();
#endif
//  	svr.AddIed(stuMmsServerIed::New(scd,0,"CG1001","S1"));
// 	svr.AddIed(stuMmsServerIed::New(scd,0,"PL1102","S1"));
// 	svr.AddIed(stuMmsServerIed::New(scd,0,"PL1112","S1"));
// 	svr.AddIed(stuMmsServerIed::New(scd,0,"PM1101A","S1"));
	//svr.AddIed(stuMmsServerIed::New(scd,0,"PM1101B","S1"));
//	svr.AddIed(stuMmsServerIed::New(scd,0,"PE1101","S1"));


// 	svr.AddIed(stuMmsServerIed::New(scd,0,"RMS601","S1"));
// 	svr.AddIed(stuMmsServerIed::New(scd,0,"CSC103BA","S1"));
// 	svr.AddIed(stuMmsServerIed::New(scd,0,"ZH5D","A1"));
// 	svr.AddIed(stuMmsServerIed::New(scd,0,"PCS921GA","S1"));
// 	svr.AddIed(stuMmsServerIed::New(scd,0,"PCS931GA","S1"));
// 	svr.AddIed(stuMmsServerIed::New(scd,0,"RCS974AA","S1"));
// 	svr.AddIed(stuMmsServerIed::New(scd,0,"PCS977DA","S1"));
// 	svr.AddIed(stuMmsServerIed::New(scd,0,"S3QSACCB1","P1"));
// 	svr.AddIed(stuMmsServerIed::New(scd,0,"S3P1PCPB1","P1"));
// 	svr.AddIed(stuMmsServerIed::New(scd,0,"WDGL_VII","S1"));
// 	svr.AddIed(stuMmsServerIed::New(scd,0,"ISA381GA","S1"));
// 	svr.AddIed(stuMmsServerIed::New(scd,0,"PRS721AA","S1"));
// 	svr.AddIed(stuMmsServerIed::New(scd,0,"PRS723CA","S1"));
// 	svr.AddIed(stuMmsServerIed::New(scd,0,"PRS753DA","S1"));
// 	svr.AddIed(stuMmsServerIed::New(scd,0,"PRS7532A","S1"));
// 	svr.AddIed(stuMmsServerIed::New(scd,0,"PRS753SA","S1"));

// 	svr.AddIed(stuMmsServerIed::New(scd,0,"PSR662UP","S1"));
// 	svr.AddIed(stuMmsServerIed::New(scd,0,"ZH5X01","A1"));
// 	svr.AddIed(stuMmsServerIed::New(scd,0,"RMS601","S1"));

// 	svr.AddIed(stuMmsServerIed::New(scd,0,"E1Q1SB1","S1"));
	svr.Start();
	printf("printf any key to quit!");
	SKT_CREATE_THREAD(ThreadRpt,&svr);

	while(1)
	{
		char ch = getchar();
		if(ch == '0')
			svr.SetGlobalCtrl_En(false,false,false);
		else if(ch == '1')
			svr.SetGlobalCtrl_En(true,true,true);
	}

	svr.Stop();

	SApi::UsSleep(1000000);
	SLog::WaitForLogEmpty();
	SLog::quitLog();
	return 0;
}

