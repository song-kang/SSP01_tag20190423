#pragma once
#include "ssp_base.h"

struct stFunc
{
	SString strIdx;
	SString strName;
	SString strDesc;
	SString strArgc;
	SString strArgcFmt;
};
struct stServer
{
	//SString strName;
	SString strIp;
	SString strPort;
};
#if 0
struct stFuncGroup
{
	SString strName;
	SPtrList<stFunc> vFuncList;
	
	stFuncGroup()
	{
		vFuncList.setAutoDelete(true);
	}
	~stFuncGroup()
	{
		vFuncList.clear();
	}
};
#endif
struct stModule
{
	SString strName;
	SString strModule;
	//SString strIp;
	SPtrList<stFunc> vFuncList;//
	stServer stConServer;
	stModule()
	{
		vFuncList.setAutoDelete(true);
	}
	~stModule()
	{
		vFuncList.clear();
	}
};

class ConfigFileMgr
{
public:
	ConfigFileMgr(void);
	~ConfigFileMgr(void);
public:
	bool readConfigFile(SString sPath);
	stModule* getModuleByName(SString& strName);
private:
	bool analysisConfigFile();
	bool readModuleNode(SBaseConfig* pParentNode,stModule* pCurModule);
	//bool readFuncGroupNode(SBaseConfig* pParentNode,stFuncGroup* pCurFuncGroup);
	void testDB();
private:
	SXmlConfig m_configFile;
	SPtrList<stModule> m_vModuleList;
};

