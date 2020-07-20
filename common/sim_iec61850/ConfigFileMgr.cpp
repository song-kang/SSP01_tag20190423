#include "ConfigFileMgr.h"
#include "ssp_base.h"
#include "SString.h"
ConfigFileMgr::ConfigFileMgr(void)
{
	m_configFile.clear();
	m_vModuleList.setAutoDelete(true);
}


ConfigFileMgr::~ConfigFileMgr(void)
{
	m_vModuleList.clear();
}

bool ConfigFileMgr::readConfigFile(SString sPath)
{
	//testDB();
	if(!m_configFile.ReadConfig(sPath))
	{
		LOGERROR("文件打开失败!file:%s", sPath.data());
		return false;
	}
	m_vModuleList.clear();
	return analysisConfigFile();
}

bool ConfigFileMgr::analysisConfigFile()
{
	unsigned long pos;
	SBaseConfig* pModuleNode = m_configFile.GetChildPtr()->FetchFirst(pos);
	while(pModuleNode)
	{
		if (pModuleNode->GetNodeName() == "Module")
		{
			stModule* pNewModule = new stModule;
			pNewModule->strName = pModuleNode->GetAttribute("name");
			pNewModule->strModule = pModuleNode->GetAttribute("module");
			//pNewModule->strIp = pModuleNode->GetAttribute("ip");
			m_vModuleList.append(pNewModule);
			readModuleNode(pModuleNode,pNewModule);
		}
		pModuleNode = m_configFile.GetChildPtr()->FetchNext(pos);
	}
	return true;
}

bool ConfigFileMgr::readModuleNode(SBaseConfig* pParentNode,stModule* pCurModule)
{
	if (pParentNode == NULL || pCurModule == NULL)
	{
		LOGERROR("传入参数错误，不能为空!");
		return false;
	}

	unsigned long pos;
	SBaseConfig *pSubNode = pParentNode->GetChildPtr()->FetchFirst(pos);
	while(pSubNode)
	{
		if(pSubNode->GetNodeName() == "func")
		{
			stFunc* pNewFunc = new stFunc;
			pNewFunc->strName = pSubNode->GetAttribute("name");
			pNewFunc->strIdx = pSubNode->GetAttribute("idx");
			pNewFunc->strDesc = pSubNode->GetAttribute("desc");
			pNewFunc->strArgc = pSubNode->GetAttribute("arg_name");
			pNewFunc->strArgcFmt = pSubNode->GetAttribute("argc_fmt");
			pCurModule->vFuncList.append(pNewFunc);
		} else if (pSubNode->GetNodeName() == "server")
		{
			pCurModule->stConServer.strIp = pSubNode->GetAttribute("ip");
			//pCurModule->stConServer.strName = pSubNode->GetAttribute("name");
			pCurModule->stConServer.strPort = pSubNode->GetAttribute("port");
		}
		pSubNode = pParentNode->GetChildPtr()->FetchNext(pos);
	}
	return true;
}
#if 0
bool ConfigFileMgr::readFuncGroupNode(SBaseConfig* pParentNode,stFuncGroup* pCurFuncGroup)
{
	if (pParentNode == NULL || pCurFuncGroup == NULL)
	{
		LOGERROR("传入参数错误，不能为空!");
		return false;
	}

	unsigned long pos;
	SBaseConfig *pSubNode = pParentNode->GetChildPtr()->FetchFirst(pos);
	while(pSubNode)
	{
		if(pSubNode->GetNodeName() == "func")
		{
			stFunc* pNewFunc = new stFunc;
			pNewFunc->strName = pSubNode->GetAttribute("name");
			pNewFunc->strIdx = pSubNode->GetAttribute("idx");
			pNewFunc->strDesc = pSubNode->GetAttribute("desc");
			pNewFunc->strArgc = pSubNode->GetAttribute("argc");
			pNewFunc->strArgcFmt = pSubNode->GetAttribute("argc");
			pCurFuncGroup->vFuncList.append(pNewFunc);
		} 
		pSubNode = pParentNode->GetChildPtr()->FetchNext(pos);
	}
	return true;
}
#endif
stModule* ConfigFileMgr::getModuleByName(SString& strName)
{
	for (int i = 0; i < m_vModuleList.count(); i++)
	{
		stModule* pCur = m_vModuleList.at(i);
		if (pCur == NULL)
		{
			continue;
		}
		if (pCur->strName == strName)
		{
			return pCur;
		}
	}
	return NULL;
}
#include <QDebug>
void ConfigFileMgr::testDB()
{
	SString sql="select sub_no from t_cim_substation";
	//sql.sprintf("select sub_no from t_cim_substation");
	SRecordset rsHis;
	
	int iNum =DB->Retrieve(sql,rsHis);
	for (int i = 0; i < iNum; i++)
	{
		int iSubno= rsHis.GetValue(i,0).toInt();
		qDebug() << "厂站ID：" << iSubno;
	}

}