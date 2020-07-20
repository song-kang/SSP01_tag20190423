/**
*
* 文 件 名 : sp_sclparser.cpp
* 创建日期 : 2015-12-22 09:02
* 作    者 : SspAssist(skt001@163.com)
* 修改日期 : $Date: 2015-12-22$
* 当前版本 : $Revision: 1.0.0$
* 功能描述 : 基于SSaxXmlEx类的XML解释功能封装
* 修改记录 : 
*            $Log: $
*
* Ver  Date        Author  Comments
* ---  ----------  ------  -------------------------------------------
* 001  2015-12-22        　创建文件
*
**/

#include "sp_sclparser.h"
unsigned char g_sCSpSclParserZero[1024];
CSpSclParser::CSpSclParser()
{
	memset(&g_sCSpSclParserZero,0,sizeof(g_sCSpSclParserZero));
	SConstPtrList<stuSCL_Private> m_Private;
	SConstPtrList<stuSCL_Header> m_Header;
	SConstPtrList<stuSCL_Substation> m_Substation;
	SConstPtrList<stuSCL_Communication> m_Communication;
	SConstPtrList<stuSCL_IED> m_IED;
	SConstPtrList<stuSCL_DataTypeTemplates> m_DataTypeTemplates;

	SetRootNodeName("SCL");
	SetRootCallbackFun(CB_SCL);
}
CSpSclParser::~CSpSclParser()
{
}
stuSaxChoice* CSpSclParser::CB_SCL(void* cbParam, SSaxAttribute *pAttribute,int iAttributeCount,char* sNodeValue)
{
	S_UNUSED(cbParam);
	S_UNUSED(pAttribute);
	S_UNUSED(iAttributeCount);
	S_UNUSED(sNodeValue);
	S_UNUSED(pAttribute);S_UNUSED(iAttributeCount);S_UNUSED(sNodeValue);static const stuSaxChoice pChoice[] = 
	{
		{ "Private",                        CB_SCL_Private },
		{ "Header",                         CB_SCL_Header },
		{ "Substation",                     CB_SCL_Substation },
		{ "Communication",                  CB_SCL_Communication },
		{ "IED",                            CB_SCL_IED },
		{ "DataTypeTemplates",              CB_SCL_DataTypeTemplates },
		{ NULL,NULL }
	};

	//CSpSclParser *pThis = (CSpSclParser*)cbParam;

	return (stuSaxChoice*)pChoice;
}

stuSaxChoice* CSpSclParser::CB_SCL_Private(void* cbParam, SSaxAttribute *pAttribute,int iAttributeCount,char* sNodeValue)
{
	S_UNUSED(pAttribute);S_UNUSED(iAttributeCount);S_UNUSED(sNodeValue);static const stuSaxChoice pChoice[] = 
	{
		{ NULL,NULL }
	};

	CSpSclParser *pThis = (CSpSclParser*)cbParam;
	pThis->m_p_SCL_Private = (stuSCL_Private*)pThis->m_ConstString.CopyToConstBuffer(g_sCSpSclParserZero,sizeof(stuSCL_Private));
	for(int i=0;i<iAttributeCount;i++,pAttribute++)
	{
		if(SString::equals(pAttribute->m_sAttrName.data(), "type"))
			pThis->m_p_SCL_Private->type = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
	}
	pThis->m_p_SCL_Private->node_value = pThis->m_ConstString.CopyToConstString(sNodeValue);
	pThis->m_Private.append(pThis->m_ConstString,pThis->m_p_SCL_Private);

	return (stuSaxChoice*)pChoice;
}

stuSaxChoice* CSpSclParser::CB_SCL_Header(void* cbParam, SSaxAttribute *pAttribute,int iAttributeCount,char* sNodeValue)
{
	S_UNUSED(pAttribute);S_UNUSED(iAttributeCount);S_UNUSED(sNodeValue);static const stuSaxChoice pChoice[] = 
	{
		{ "History",                        CB_SCL_Header_History },
		{ NULL,NULL }
	};

	CSpSclParser *pThis = (CSpSclParser*)cbParam;
	pThis->m_p_SCL_Header = (stuSCL_Header*)pThis->m_ConstString.CopyToConstBuffer(g_sCSpSclParserZero,sizeof(stuSCL_Header));
	for(int i=0;i<iAttributeCount;i++,pAttribute++)
	{
		if(SString::equals(pAttribute->m_sAttrName.data(), "id"))
			pThis->m_p_SCL_Header->id = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
		else if(SString::equals(pAttribute->m_sAttrName.data(), "version"))
			pThis->m_p_SCL_Header->version = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
		else if(SString::equals(pAttribute->m_sAttrName.data(), "revision"))
			pThis->m_p_SCL_Header->revision = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
		else if(SString::equals(pAttribute->m_sAttrName.data(), "toolID"))
			pThis->m_p_SCL_Header->toolID = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
		else if(SString::equals(pAttribute->m_sAttrName.data(), "nameStructure"))
			pThis->m_p_SCL_Header->nameStructure = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
	}
	pThis->m_p_SCL_Header->node_value = pThis->m_ConstString.CopyToConstString(sNodeValue);
	pThis->m_Header.append(pThis->m_ConstString,pThis->m_p_SCL_Header);

	return (stuSaxChoice*)pChoice;
}

stuSaxChoice* CSpSclParser::CB_SCL_Header_History(void* cbParam, SSaxAttribute *pAttribute,int iAttributeCount,char* sNodeValue)
{
	S_UNUSED(pAttribute);
	S_UNUSED(iAttributeCount);
	S_UNUSED(pAttribute);S_UNUSED(iAttributeCount);S_UNUSED(sNodeValue);static const stuSaxChoice pChoice[] = 
	{
		{ "Hitem",                          CB_SCL_Header_History_Hitem },
		{ NULL,NULL }
	};

	CSpSclParser *pThis = (CSpSclParser*)cbParam;
	pThis->m_p_SCL_Header_History = (stuSCL_Header_History*)pThis->m_ConstString.CopyToConstBuffer(g_sCSpSclParserZero,sizeof(stuSCL_Header_History));
	pThis->m_p_SCL_Header_History->node_value = pThis->m_ConstString.CopyToConstString(sNodeValue);
	pThis->m_p_SCL_Header->m_History.append(pThis->m_ConstString,pThis->m_p_SCL_Header_History);

	return (stuSaxChoice*)pChoice;
}

stuSaxChoice* CSpSclParser::CB_SCL_Header_History_Hitem(void* cbParam, SSaxAttribute *pAttribute,int iAttributeCount,char* sNodeValue)
{
	S_UNUSED(pAttribute);S_UNUSED(iAttributeCount);S_UNUSED(sNodeValue);static const stuSaxChoice pChoice[] = 
	{
		{ NULL,NULL }
	};

	CSpSclParser *pThis = (CSpSclParser*)cbParam;
	pThis->m_p_SCL_Header_History_Hitem = (stuSCL_Header_History_Hitem*)pThis->m_ConstString.CopyToConstBuffer(g_sCSpSclParserZero,sizeof(stuSCL_Header_History_Hitem));
	for(int i=0;i<iAttributeCount;i++,pAttribute++)
	{
		if(SString::equals(pAttribute->m_sAttrName.data(), "version"))
			pThis->m_p_SCL_Header_History_Hitem->version = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
		else if(SString::equals(pAttribute->m_sAttrName.data(), "revision"))
			pThis->m_p_SCL_Header_History_Hitem->revision = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
		else if(SString::equals(pAttribute->m_sAttrName.data(), "when"))
			pThis->m_p_SCL_Header_History_Hitem->when = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
		else if(SString::equals(pAttribute->m_sAttrName.data(), "who"))
			pThis->m_p_SCL_Header_History_Hitem->who = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
		else if(SString::equals(pAttribute->m_sAttrName.data(), "what"))
			pThis->m_p_SCL_Header_History_Hitem->what = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
		else if(SString::equals(pAttribute->m_sAttrName.data(), "why"))
			pThis->m_p_SCL_Header_History_Hitem->why = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
	}
	pThis->m_p_SCL_Header_History_Hitem->node_value = pThis->m_ConstString.CopyToConstString(sNodeValue);
	pThis->m_p_SCL_Header_History->m_Hitem.append(pThis->m_ConstString,pThis->m_p_SCL_Header_History_Hitem);

	return (stuSaxChoice*)pChoice;
}

stuSaxChoice* CSpSclParser::CB_SCL_Substation(void* cbParam, SSaxAttribute *pAttribute,int iAttributeCount,char* sNodeValue)
{
	S_UNUSED(pAttribute);S_UNUSED(iAttributeCount);S_UNUSED(sNodeValue);static const stuSaxChoice pChoice[] = 
	{
		{ "VoltageLevel",                   CB_SCL_Substation_VoltageLevel },
		{ NULL,NULL }
	};

	CSpSclParser *pThis = (CSpSclParser*)cbParam;
	pThis->m_p_SCL_Substation = (stuSCL_Substation*)pThis->m_ConstString.CopyToConstBuffer(g_sCSpSclParserZero,sizeof(stuSCL_Substation));
	for(int i=0;i<iAttributeCount;i++,pAttribute++)
	{
		if(SString::equals(pAttribute->m_sAttrName.data(), "desc"))
			pThis->m_p_SCL_Substation->desc = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
		else if(SString::equals(pAttribute->m_sAttrName.data(), "name"))
			pThis->m_p_SCL_Substation->name = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
	}
	pThis->m_p_SCL_Substation->node_value = pThis->m_ConstString.CopyToConstString(sNodeValue);
	pThis->m_Substation.append(pThis->m_ConstString,pThis->m_p_SCL_Substation);

	return (stuSaxChoice*)pChoice;
}

stuSaxChoice* CSpSclParser::CB_SCL_Substation_VoltageLevel(void* cbParam, SSaxAttribute *pAttribute,int iAttributeCount,char* sNodeValue)
{
	S_UNUSED(pAttribute);S_UNUSED(iAttributeCount);S_UNUSED(sNodeValue);static const stuSaxChoice pChoice[] = 
	{
		{ "Bay",                            CB_SCL_Substation_VoltageLevel_Bay },
		{ NULL,NULL }
	};

	CSpSclParser *pThis = (CSpSclParser*)cbParam;
	pThis->m_p_SCL_Substation_VoltageLevel = (stuSCL_Substation_VoltageLevel*)pThis->m_ConstString.CopyToConstBuffer(g_sCSpSclParserZero,sizeof(stuSCL_Substation_VoltageLevel));
	for(int i=0;i<iAttributeCount;i++,pAttribute++)
	{
		if(SString::equals(pAttribute->m_sAttrName.data(), "desc"))
			pThis->m_p_SCL_Substation_VoltageLevel->desc = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
		else if(SString::equals(pAttribute->m_sAttrName.data(), "name"))
			pThis->m_p_SCL_Substation_VoltageLevel->name = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
	}
	pThis->m_p_SCL_Substation_VoltageLevel->node_value = pThis->m_ConstString.CopyToConstString(sNodeValue);
	pThis->m_p_SCL_Substation->m_VoltageLevel.append(pThis->m_ConstString,pThis->m_p_SCL_Substation_VoltageLevel);

	return (stuSaxChoice*)pChoice;
}

stuSaxChoice* CSpSclParser::CB_SCL_Substation_VoltageLevel_Bay(void* cbParam, SSaxAttribute *pAttribute,int iAttributeCount,char* sNodeValue)
{
	S_UNUSED(pAttribute);S_UNUSED(iAttributeCount);S_UNUSED(sNodeValue);static const stuSaxChoice pChoice[] = 
	{
		{ NULL,NULL }
	};

	CSpSclParser *pThis = (CSpSclParser*)cbParam;
	pThis->m_p_SCL_Substation_VoltageLevel_Bay = (stuSCL_Substation_VoltageLevel_Bay*)pThis->m_ConstString.CopyToConstBuffer(g_sCSpSclParserZero,sizeof(stuSCL_Substation_VoltageLevel_Bay));
	for(int i=0;i<iAttributeCount;i++,pAttribute++)
	{
		if(SString::equals(pAttribute->m_sAttrName.data(), "name"))
			pThis->m_p_SCL_Substation_VoltageLevel_Bay->name = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
	}
	pThis->m_p_SCL_Substation_VoltageLevel_Bay->node_value = pThis->m_ConstString.CopyToConstString(sNodeValue);
	pThis->m_p_SCL_Substation_VoltageLevel->m_Bay.append(pThis->m_ConstString,pThis->m_p_SCL_Substation_VoltageLevel_Bay);

	return (stuSaxChoice*)pChoice;
}

stuSaxChoice* CSpSclParser::CB_SCL_Communication(void* cbParam, SSaxAttribute *pAttribute,int iAttributeCount,char* sNodeValue)
{
	S_UNUSED(pAttribute);S_UNUSED(iAttributeCount);S_UNUSED(sNodeValue);static const stuSaxChoice pChoice[] = 
	{
		{ "SubNetwork",                     CB_SCL_Communication_SubNetwork },
		{ NULL,NULL }
	};

	CSpSclParser *pThis = (CSpSclParser*)cbParam;
	pThis->m_p_SCL_Communication = (stuSCL_Communication*)pThis->m_ConstString.CopyToConstBuffer(g_sCSpSclParserZero,sizeof(stuSCL_Communication));
	for(int i=0;i<iAttributeCount;i++,pAttribute++)
	{
		if(SString::equals(pAttribute->m_sAttrName.data(), "desc"))
			pThis->m_p_SCL_Communication->desc = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
	}
	pThis->m_p_SCL_Communication->node_value = pThis->m_ConstString.CopyToConstString(sNodeValue);
	pThis->m_Communication.append(pThis->m_ConstString,pThis->m_p_SCL_Communication);

	return (stuSaxChoice*)pChoice;
}

stuSaxChoice* CSpSclParser::CB_SCL_Communication_SubNetwork(void* cbParam, SSaxAttribute *pAttribute,int iAttributeCount,char* sNodeValue)
{
	S_UNUSED(pAttribute);S_UNUSED(iAttributeCount);S_UNUSED(sNodeValue);static const stuSaxChoice pChoice[] = 
	{
		{ "BitRate",                        CB_SCL_Communication_SubNetwork_BitRate },
		{ "ConnectedAP",                    CB_SCL_Communication_SubNetwork_ConnectedAP },
		{ NULL,NULL }
	};

	CSpSclParser *pThis = (CSpSclParser*)cbParam;
	pThis->m_p_SCL_Communication_SubNetwork = (stuSCL_Communication_SubNetwork*)pThis->m_ConstString.CopyToConstBuffer(g_sCSpSclParserZero,sizeof(stuSCL_Communication_SubNetwork));
	for(int i=0;i<iAttributeCount;i++,pAttribute++)
	{
		if(SString::equals(pAttribute->m_sAttrName.data(), "name"))
			pThis->m_p_SCL_Communication_SubNetwork->name = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
		else if(SString::equals(pAttribute->m_sAttrName.data(), "desc"))
			pThis->m_p_SCL_Communication_SubNetwork->desc = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
		else if(SString::equals(pAttribute->m_sAttrName.data(), "type"))
			pThis->m_p_SCL_Communication_SubNetwork->type = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
	}
	pThis->m_p_SCL_Communication_SubNetwork->node_value = pThis->m_ConstString.CopyToConstString(sNodeValue);
	pThis->m_p_SCL_Communication->m_SubNetwork.append(pThis->m_ConstString,pThis->m_p_SCL_Communication_SubNetwork);

	return (stuSaxChoice*)pChoice;
}

stuSaxChoice* CSpSclParser::CB_SCL_Communication_SubNetwork_BitRate(void* cbParam, SSaxAttribute *pAttribute,int iAttributeCount,char* sNodeValue)
{
	S_UNUSED(pAttribute);S_UNUSED(iAttributeCount);S_UNUSED(sNodeValue);static const stuSaxChoice pChoice[] = 
	{
		{ NULL,NULL }
	};

	CSpSclParser *pThis = (CSpSclParser*)cbParam;
	pThis->m_p_SCL_Communication_SubNetwork_BitRate = (stuSCL_Communication_SubNetwork_BitRate*)pThis->m_ConstString.CopyToConstBuffer(g_sCSpSclParserZero,sizeof(stuSCL_Communication_SubNetwork_BitRate));
	for(int i=0;i<iAttributeCount;i++,pAttribute++)
	{
		if(SString::equals(pAttribute->m_sAttrName.data(), "unit"))
			pThis->m_p_SCL_Communication_SubNetwork_BitRate->unit = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
		else if(SString::equals(pAttribute->m_sAttrName.data(), "multiplier"))
			pThis->m_p_SCL_Communication_SubNetwork_BitRate->multiplier = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
	}
	pThis->m_p_SCL_Communication_SubNetwork_BitRate->node_value = pThis->m_ConstString.CopyToConstString(sNodeValue);
	pThis->m_p_SCL_Communication_SubNetwork->m_BitRate.append(pThis->m_ConstString,pThis->m_p_SCL_Communication_SubNetwork_BitRate);

	return (stuSaxChoice*)pChoice;
}

stuSaxChoice* CSpSclParser::CB_SCL_Communication_SubNetwork_ConnectedAP(void* cbParam, SSaxAttribute *pAttribute,int iAttributeCount,char* sNodeValue)
{
	S_UNUSED(pAttribute);S_UNUSED(iAttributeCount);S_UNUSED(sNodeValue);static const stuSaxChoice pChoice[] = 
	{
		{ "Private",                        CB_SCL_Communication_SubNetwork_ConnectedAP_Private },
		{ "Address",                        CB_SCL_Communication_SubNetwork_ConnectedAP_Address },
		{ "GSE",                            CB_SCL_Communication_SubNetwork_ConnectedAP_GSE },
		{ "SMV",                            CB_SCL_Communication_SubNetwork_ConnectedAP_SMV },
		{ NULL,NULL }
	};

	CSpSclParser *pThis = (CSpSclParser*)cbParam;
	pThis->m_p_SCL_Communication_SubNetwork_ConnectedAP = (stuSCL_Communication_SubNetwork_ConnectedAP*)pThis->m_ConstString.CopyToConstBuffer(g_sCSpSclParserZero,sizeof(stuSCL_Communication_SubNetwork_ConnectedAP));
	for(int i=0;i<iAttributeCount;i++,pAttribute++)
	{
		if(SString::equals(pAttribute->m_sAttrName.data(), "iedName"))
			pThis->m_p_SCL_Communication_SubNetwork_ConnectedAP->iedName = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
		else if(SString::equals(pAttribute->m_sAttrName.data(), "apName"))
			pThis->m_p_SCL_Communication_SubNetwork_ConnectedAP->apName = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
		else if(SString::equals(pAttribute->m_sAttrName.data(), "desc"))
			pThis->m_p_SCL_Communication_SubNetwork_ConnectedAP->desc = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
	}
	pThis->m_p_SCL_Communication_SubNetwork_ConnectedAP->node_value = pThis->m_ConstString.CopyToConstString(sNodeValue);
	pThis->m_p_SCL_Communication_SubNetwork->m_ConnectedAP.append(pThis->m_ConstString,pThis->m_p_SCL_Communication_SubNetwork_ConnectedAP);

	return (stuSaxChoice*)pChoice;
}

stuSaxChoice* CSpSclParser::CB_SCL_Communication_SubNetwork_ConnectedAP_Private(void* cbParam, SSaxAttribute *pAttribute,int iAttributeCount,char* sNodeValue)
{
	S_UNUSED(pAttribute);S_UNUSED(iAttributeCount);S_UNUSED(sNodeValue);static const stuSaxChoice pChoice[] = 
	{
		{ NULL,NULL }
	};

	CSpSclParser *pThis = (CSpSclParser*)cbParam;
	pThis->m_p_SCL_Communication_SubNetwork_ConnectedAP_Private = (stuSCL_Communication_SubNetwork_ConnectedAP_Private*)pThis->m_ConstString.CopyToConstBuffer(g_sCSpSclParserZero,sizeof(stuSCL_Communication_SubNetwork_ConnectedAP_Private));
	for(int i=0;i<iAttributeCount;i++,pAttribute++)
	{
		if(SString::equals(pAttribute->m_sAttrName.data(), "type"))
			pThis->m_p_SCL_Communication_SubNetwork_ConnectedAP_Private->type = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
	}
	pThis->m_p_SCL_Communication_SubNetwork_ConnectedAP_Private->node_value = pThis->m_ConstString.CopyToConstString(sNodeValue);
	pThis->m_p_SCL_Communication_SubNetwork_ConnectedAP->m_Private.append(pThis->m_ConstString,pThis->m_p_SCL_Communication_SubNetwork_ConnectedAP_Private);

	return (stuSaxChoice*)pChoice;
}

stuSaxChoice* CSpSclParser::CB_SCL_Communication_SubNetwork_ConnectedAP_Address(void* cbParam, SSaxAttribute *pAttribute,int iAttributeCount,char* sNodeValue)
{
	S_UNUSED(pAttribute);
	S_UNUSED(iAttributeCount);
	S_UNUSED(pAttribute);S_UNUSED(iAttributeCount);S_UNUSED(sNodeValue);static const stuSaxChoice pChoice[] = 
	{
		{ "P",                              CB_SCL_Communication_SubNetwork_ConnectedAP_Address_P },
		{ NULL,NULL }
	};

	CSpSclParser *pThis = (CSpSclParser*)cbParam;
	pThis->m_p_SCL_Communication_SubNetwork_ConnectedAP_Address = (stuSCL_Communication_SubNetwork_ConnectedAP_Address*)pThis->m_ConstString.CopyToConstBuffer(g_sCSpSclParserZero,sizeof(stuSCL_Communication_SubNetwork_ConnectedAP_Address));
	pThis->m_p_SCL_Communication_SubNetwork_ConnectedAP_Address->node_value = pThis->m_ConstString.CopyToConstString(sNodeValue);
	pThis->m_p_SCL_Communication_SubNetwork_ConnectedAP->m_Address.append(pThis->m_ConstString,pThis->m_p_SCL_Communication_SubNetwork_ConnectedAP_Address);

	return (stuSaxChoice*)pChoice;
}

stuSaxChoice* CSpSclParser::CB_SCL_Communication_SubNetwork_ConnectedAP_Address_P(void* cbParam, SSaxAttribute *pAttribute,int iAttributeCount,char* sNodeValue)
{
	S_UNUSED(pAttribute);S_UNUSED(iAttributeCount);S_UNUSED(sNodeValue);static const stuSaxChoice pChoice[] = 
	{
		{ NULL,NULL }
	};

	CSpSclParser *pThis = (CSpSclParser*)cbParam;
	pThis->m_p_SCL_Communication_SubNetwork_ConnectedAP_Address_P = (stuSCL_Communication_SubNetwork_ConnectedAP_Address_P*)pThis->m_ConstString.CopyToConstBuffer(g_sCSpSclParserZero,sizeof(stuSCL_Communication_SubNetwork_ConnectedAP_Address_P));
	for(int i=0;i<iAttributeCount;i++,pAttribute++)
	{
		if(SString::equals(pAttribute->m_sAttrName.data(), "type"))
			pThis->m_p_SCL_Communication_SubNetwork_ConnectedAP_Address_P->type = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
	}
	pThis->m_p_SCL_Communication_SubNetwork_ConnectedAP_Address_P->node_value = pThis->m_ConstString.CopyToConstString(sNodeValue);
	pThis->m_p_SCL_Communication_SubNetwork_ConnectedAP_Address->m_P.append(pThis->m_ConstString,pThis->m_p_SCL_Communication_SubNetwork_ConnectedAP_Address_P);

	return (stuSaxChoice*)pChoice;
}

stuSaxChoice* CSpSclParser::CB_SCL_Communication_SubNetwork_ConnectedAP_GSE(void* cbParam, SSaxAttribute *pAttribute,int iAttributeCount,char* sNodeValue)
{
	S_UNUSED(pAttribute);S_UNUSED(iAttributeCount);S_UNUSED(sNodeValue);static const stuSaxChoice pChoice[] = 
	{
		{ "Address",                        CB_SCL_Communication_SubNetwork_ConnectedAP_GSE_Address },
		{ "MinTime",                        CB_SCL_Communication_SubNetwork_ConnectedAP_GSE_MinTime },
		{ "MaxTime",                        CB_SCL_Communication_SubNetwork_ConnectedAP_GSE_MaxTime },
		{ NULL,NULL }
	};

	CSpSclParser *pThis = (CSpSclParser*)cbParam;
	pThis->m_p_SCL_Communication_SubNetwork_ConnectedAP_GSE = (stuSCL_Communication_SubNetwork_ConnectedAP_GSE*)pThis->m_ConstString.CopyToConstBuffer(g_sCSpSclParserZero,sizeof(stuSCL_Communication_SubNetwork_ConnectedAP_GSE));
	for(int i=0;i<iAttributeCount;i++,pAttribute++)
	{
		if(SString::equals(pAttribute->m_sAttrName.data(), "cbName"))
			pThis->m_p_SCL_Communication_SubNetwork_ConnectedAP_GSE->cbName = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
		else if(SString::equals(pAttribute->m_sAttrName.data(), "ldInst"))
			pThis->m_p_SCL_Communication_SubNetwork_ConnectedAP_GSE->ldInst = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
	}
	pThis->m_p_SCL_Communication_SubNetwork_ConnectedAP_GSE->node_value = pThis->m_ConstString.CopyToConstString(sNodeValue);
	pThis->m_p_SCL_Communication_SubNetwork_ConnectedAP->m_GSE.append(pThis->m_ConstString,pThis->m_p_SCL_Communication_SubNetwork_ConnectedAP_GSE);

	return (stuSaxChoice*)pChoice;
}

stuSaxChoice* CSpSclParser::CB_SCL_Communication_SubNetwork_ConnectedAP_GSE_Address(void* cbParam, SSaxAttribute *pAttribute,int iAttributeCount,char* sNodeValue)
{
	S_UNUSED(pAttribute);
	S_UNUSED(iAttributeCount);
	S_UNUSED(pAttribute);S_UNUSED(iAttributeCount);S_UNUSED(sNodeValue);static const stuSaxChoice pChoice[] = 
	{
		{ "P",                              CB_SCL_Communication_SubNetwork_ConnectedAP_GSE_Address_P },
		{ NULL,NULL }
	};

	CSpSclParser *pThis = (CSpSclParser*)cbParam;
	pThis->m_p_SCL_Communication_SubNetwork_ConnectedAP_GSE_Address = (stuSCL_Communication_SubNetwork_ConnectedAP_GSE_Address*)pThis->m_ConstString.CopyToConstBuffer(g_sCSpSclParserZero,sizeof(stuSCL_Communication_SubNetwork_ConnectedAP_GSE_Address));
	pThis->m_p_SCL_Communication_SubNetwork_ConnectedAP_GSE_Address->node_value = pThis->m_ConstString.CopyToConstString(sNodeValue);
	pThis->m_p_SCL_Communication_SubNetwork_ConnectedAP_GSE->m_Address.append(pThis->m_ConstString,pThis->m_p_SCL_Communication_SubNetwork_ConnectedAP_GSE_Address);

	return (stuSaxChoice*)pChoice;
}

stuSaxChoice* CSpSclParser::CB_SCL_Communication_SubNetwork_ConnectedAP_GSE_Address_P(void* cbParam, SSaxAttribute *pAttribute,int iAttributeCount,char* sNodeValue)
{
	S_UNUSED(pAttribute);S_UNUSED(iAttributeCount);S_UNUSED(sNodeValue);static const stuSaxChoice pChoice[] = 
	{
		{ NULL,NULL }
	};

	CSpSclParser *pThis = (CSpSclParser*)cbParam;
	pThis->m_p_SCL_Communication_SubNetwork_ConnectedAP_GSE_Address_P = (stuSCL_Communication_SubNetwork_ConnectedAP_GSE_Address_P*)pThis->m_ConstString.CopyToConstBuffer(g_sCSpSclParserZero,sizeof(stuSCL_Communication_SubNetwork_ConnectedAP_GSE_Address_P));
	for(int i=0;i<iAttributeCount;i++,pAttribute++)
	{
		if(SString::equals(pAttribute->m_sAttrName.data(), "type"))
			pThis->m_p_SCL_Communication_SubNetwork_ConnectedAP_GSE_Address_P->type = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
	}
	pThis->m_p_SCL_Communication_SubNetwork_ConnectedAP_GSE_Address_P->node_value = pThis->m_ConstString.CopyToConstString(sNodeValue);
	pThis->m_p_SCL_Communication_SubNetwork_ConnectedAP_GSE_Address->m_P.append(pThis->m_ConstString,pThis->m_p_SCL_Communication_SubNetwork_ConnectedAP_GSE_Address_P);

	return (stuSaxChoice*)pChoice;
}

stuSaxChoice* CSpSclParser::CB_SCL_Communication_SubNetwork_ConnectedAP_GSE_MinTime(void* cbParam, SSaxAttribute *pAttribute,int iAttributeCount,char* sNodeValue)
{
	S_UNUSED(pAttribute);S_UNUSED(iAttributeCount);S_UNUSED(sNodeValue);static const stuSaxChoice pChoice[] = 
	{
		{ NULL,NULL }
	};

	CSpSclParser *pThis = (CSpSclParser*)cbParam;
	pThis->m_p_SCL_Communication_SubNetwork_ConnectedAP_GSE_MinTime = (stuSCL_Communication_SubNetwork_ConnectedAP_GSE_MinTime*)pThis->m_ConstString.CopyToConstBuffer(g_sCSpSclParserZero,sizeof(stuSCL_Communication_SubNetwork_ConnectedAP_GSE_MinTime));
	for(int i=0;i<iAttributeCount;i++,pAttribute++)
	{
		if(SString::equals(pAttribute->m_sAttrName.data(), "unit"))
			pThis->m_p_SCL_Communication_SubNetwork_ConnectedAP_GSE_MinTime->unit = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
		else if(SString::equals(pAttribute->m_sAttrName.data(), "multiplier"))
			pThis->m_p_SCL_Communication_SubNetwork_ConnectedAP_GSE_MinTime->multiplier = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
	}
	pThis->m_p_SCL_Communication_SubNetwork_ConnectedAP_GSE_MinTime->node_value = pThis->m_ConstString.CopyToConstString(sNodeValue);
	pThis->m_p_SCL_Communication_SubNetwork_ConnectedAP_GSE->m_MinTime.append(pThis->m_ConstString,pThis->m_p_SCL_Communication_SubNetwork_ConnectedAP_GSE_MinTime);

	return (stuSaxChoice*)pChoice;
}

stuSaxChoice* CSpSclParser::CB_SCL_Communication_SubNetwork_ConnectedAP_GSE_MaxTime(void* cbParam, SSaxAttribute *pAttribute,int iAttributeCount,char* sNodeValue)
{
	S_UNUSED(pAttribute);S_UNUSED(iAttributeCount);S_UNUSED(sNodeValue);static const stuSaxChoice pChoice[] = 
	{
		{ NULL,NULL }
	};

	CSpSclParser *pThis = (CSpSclParser*)cbParam;
	pThis->m_p_SCL_Communication_SubNetwork_ConnectedAP_GSE_MaxTime = (stuSCL_Communication_SubNetwork_ConnectedAP_GSE_MaxTime*)pThis->m_ConstString.CopyToConstBuffer(g_sCSpSclParserZero,sizeof(stuSCL_Communication_SubNetwork_ConnectedAP_GSE_MaxTime));
	for(int i=0;i<iAttributeCount;i++,pAttribute++)
	{
		if(SString::equals(pAttribute->m_sAttrName.data(), "unit"))
			pThis->m_p_SCL_Communication_SubNetwork_ConnectedAP_GSE_MaxTime->unit = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
		else if(SString::equals(pAttribute->m_sAttrName.data(), "multiplier"))
			pThis->m_p_SCL_Communication_SubNetwork_ConnectedAP_GSE_MaxTime->multiplier = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
	}
	pThis->m_p_SCL_Communication_SubNetwork_ConnectedAP_GSE_MaxTime->node_value = pThis->m_ConstString.CopyToConstString(sNodeValue);
	pThis->m_p_SCL_Communication_SubNetwork_ConnectedAP_GSE->m_MaxTime.append(pThis->m_ConstString,pThis->m_p_SCL_Communication_SubNetwork_ConnectedAP_GSE_MaxTime);

	return (stuSaxChoice*)pChoice;
}

stuSaxChoice* CSpSclParser::CB_SCL_Communication_SubNetwork_ConnectedAP_SMV(void* cbParam, SSaxAttribute *pAttribute,int iAttributeCount,char* sNodeValue)
{
	S_UNUSED(pAttribute);S_UNUSED(iAttributeCount);S_UNUSED(sNodeValue);static const stuSaxChoice pChoice[] = 
	{
		{ "Address",                        CB_SCL_Communication_SubNetwork_ConnectedAP_SMV_Address },
		{ NULL,NULL }
	};

	CSpSclParser *pThis = (CSpSclParser*)cbParam;
	pThis->m_p_SCL_Communication_SubNetwork_ConnectedAP_SMV = (stuSCL_Communication_SubNetwork_ConnectedAP_SMV*)pThis->m_ConstString.CopyToConstBuffer(g_sCSpSclParserZero,sizeof(stuSCL_Communication_SubNetwork_ConnectedAP_SMV));
	for(int i=0;i<iAttributeCount;i++,pAttribute++)
	{
		if(SString::equals(pAttribute->m_sAttrName.data(), "cbName"))
			pThis->m_p_SCL_Communication_SubNetwork_ConnectedAP_SMV->cbName = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
		else if(SString::equals(pAttribute->m_sAttrName.data(), "ldInst"))
			pThis->m_p_SCL_Communication_SubNetwork_ConnectedAP_SMV->ldInst = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
	}
	pThis->m_p_SCL_Communication_SubNetwork_ConnectedAP_SMV->node_value = pThis->m_ConstString.CopyToConstString(sNodeValue);
	pThis->m_p_SCL_Communication_SubNetwork_ConnectedAP->m_SMV.append(pThis->m_ConstString,pThis->m_p_SCL_Communication_SubNetwork_ConnectedAP_SMV);

	return (stuSaxChoice*)pChoice;
}

stuSaxChoice* CSpSclParser::CB_SCL_Communication_SubNetwork_ConnectedAP_SMV_Address(void* cbParam, SSaxAttribute *pAttribute,int iAttributeCount,char* sNodeValue)
{
	S_UNUSED(pAttribute);
	S_UNUSED(iAttributeCount);
	S_UNUSED(pAttribute);S_UNUSED(iAttributeCount);S_UNUSED(sNodeValue);static const stuSaxChoice pChoice[] = 
	{
		{ "P",                              CB_SCL_Communication_SubNetwork_ConnectedAP_SMV_Address_P },
		{ NULL,NULL }
	};

	CSpSclParser *pThis = (CSpSclParser*)cbParam;
	pThis->m_p_SCL_Communication_SubNetwork_ConnectedAP_SMV_Address = (stuSCL_Communication_SubNetwork_ConnectedAP_SMV_Address*)pThis->m_ConstString.CopyToConstBuffer(g_sCSpSclParserZero,sizeof(stuSCL_Communication_SubNetwork_ConnectedAP_SMV_Address));
	pThis->m_p_SCL_Communication_SubNetwork_ConnectedAP_SMV_Address->node_value = pThis->m_ConstString.CopyToConstString(sNodeValue);
	pThis->m_p_SCL_Communication_SubNetwork_ConnectedAP_SMV->m_Address.append(pThis->m_ConstString,pThis->m_p_SCL_Communication_SubNetwork_ConnectedAP_SMV_Address);

	return (stuSaxChoice*)pChoice;
}

stuSaxChoice* CSpSclParser::CB_SCL_Communication_SubNetwork_ConnectedAP_SMV_Address_P(void* cbParam, SSaxAttribute *pAttribute,int iAttributeCount,char* sNodeValue)
{
	S_UNUSED(pAttribute);S_UNUSED(iAttributeCount);S_UNUSED(sNodeValue);static const stuSaxChoice pChoice[] = 
	{
		{ NULL,NULL }
	};

	CSpSclParser *pThis = (CSpSclParser*)cbParam;
	pThis->m_p_SCL_Communication_SubNetwork_ConnectedAP_SMV_Address_P = (stuSCL_Communication_SubNetwork_ConnectedAP_SMV_Address_P*)pThis->m_ConstString.CopyToConstBuffer(g_sCSpSclParserZero,sizeof(stuSCL_Communication_SubNetwork_ConnectedAP_SMV_Address_P));
	for(int i=0;i<iAttributeCount;i++,pAttribute++)
	{
		if(SString::equals(pAttribute->m_sAttrName.data(), "type"))
			pThis->m_p_SCL_Communication_SubNetwork_ConnectedAP_SMV_Address_P->type = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
	}
	pThis->m_p_SCL_Communication_SubNetwork_ConnectedAP_SMV_Address_P->node_value = pThis->m_ConstString.CopyToConstString(sNodeValue);
	pThis->m_p_SCL_Communication_SubNetwork_ConnectedAP_SMV_Address->m_P.append(pThis->m_ConstString,pThis->m_p_SCL_Communication_SubNetwork_ConnectedAP_SMV_Address_P);

	return (stuSaxChoice*)pChoice;
}

stuSaxChoice* CSpSclParser::CB_SCL_IED(void* cbParam, SSaxAttribute *pAttribute,int iAttributeCount,char* sNodeValue)
{
	S_UNUSED(pAttribute);S_UNUSED(iAttributeCount);S_UNUSED(sNodeValue);static const stuSaxChoice pChoice[] = 
	{
		{ "Private",                        CB_SCL_IED_Private },
		{ "Services",                       CB_SCL_IED_Services },
		{ "AccessPoint",                    CB_SCL_IED_AccessPoint },
		{ NULL,NULL }
	};

	CSpSclParser *pThis = (CSpSclParser*)cbParam;
	pThis->m_p_SCL_IED = (stuSCL_IED*)pThis->m_ConstString.CopyToConstBuffer(g_sCSpSclParserZero,sizeof(stuSCL_IED));
	for(int i=0;i<iAttributeCount;i++,pAttribute++)
	{
		if(SString::equals(pAttribute->m_sAttrName.data(), "manufacturer"))
			pThis->m_p_SCL_IED->manufacturer = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
		else if(SString::equals(pAttribute->m_sAttrName.data(), "name"))
			pThis->m_p_SCL_IED->name = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
		else if(SString::equals(pAttribute->m_sAttrName.data(), "type"))
			pThis->m_p_SCL_IED->type = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
		else if(SString::equals(pAttribute->m_sAttrName.data(), "desc"))
			pThis->m_p_SCL_IED->desc = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
		else if(SString::equals(pAttribute->m_sAttrName.data(), "configVersion"))
			pThis->m_p_SCL_IED->configVersion = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
	}
	pThis->m_p_SCL_IED->node_value = pThis->m_ConstString.CopyToConstString(sNodeValue);
	pThis->m_IED.append(pThis->m_ConstString,pThis->m_p_SCL_IED);

	return (stuSaxChoice*)pChoice;
}

stuSaxChoice* CSpSclParser::CB_SCL_IED_Private(void* cbParam, SSaxAttribute *pAttribute,int iAttributeCount,char* sNodeValue)
{
	S_UNUSED(pAttribute);S_UNUSED(iAttributeCount);S_UNUSED(sNodeValue);static const stuSaxChoice pChoice[] = 
	{
		{ NULL,NULL }
	};

	CSpSclParser *pThis = (CSpSclParser*)cbParam;
	pThis->m_p_SCL_IED_Private = (stuSCL_IED_Private*)pThis->m_ConstString.CopyToConstBuffer(g_sCSpSclParserZero,sizeof(stuSCL_IED_Private));
	for(int i=0;i<iAttributeCount;i++,pAttribute++)
	{
		if(SString::equals(pAttribute->m_sAttrName.data(), "type"))
			pThis->m_p_SCL_IED_Private->type = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
		else if(SString::equals(pAttribute->m_sAttrName.data(), "source"))
			pThis->m_p_SCL_IED_Private->source = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
	}
	pThis->m_p_SCL_IED_Private->node_value = pThis->m_ConstString.CopyToConstString(sNodeValue);
	pThis->m_p_SCL_IED->m_Private.append(pThis->m_ConstString,pThis->m_p_SCL_IED_Private);

	return (stuSaxChoice*)pChoice;
}

stuSaxChoice* CSpSclParser::CB_SCL_IED_Services(void* cbParam, SSaxAttribute *pAttribute,int iAttributeCount,char* sNodeValue)
{
	S_UNUSED(pAttribute);
	S_UNUSED(iAttributeCount);
	S_UNUSED(pAttribute);S_UNUSED(iAttributeCount);S_UNUSED(sNodeValue);static const stuSaxChoice pChoice[] = 
	{
		{ "DynAssociation",                 CB_SCL_IED_Services_DynAssociation },
		{ "SettingGroups",                  CB_SCL_IED_Services_SettingGroups },
		{ "GetDirectory",                   CB_SCL_IED_Services_GetDirectory },
		{ "GetDataObjectDefinition",        CB_SCL_IED_Services_GetDataObjectDefinition },
		{ "DataObjectDirectory",            CB_SCL_IED_Services_DataObjectDirectory },
		{ "GetDataSetValue",                CB_SCL_IED_Services_GetDataSetValue },
		{ "DataSetDirectory",               CB_SCL_IED_Services_DataSetDirectory },
		{ "ConfDataSet",                    CB_SCL_IED_Services_ConfDataSet },
		{ "ReadWrite",                      CB_SCL_IED_Services_ReadWrite },
		{ "ConfReportControl",              CB_SCL_IED_Services_ConfReportControl },
		{ "GetCBValues",                    CB_SCL_IED_Services_GetCBValues },
		{ "ReportSettings",                 CB_SCL_IED_Services_ReportSettings },
		{ "GSESettings",                    CB_SCL_IED_Services_GSESettings },
		{ "GOOSE",                          CB_SCL_IED_Services_GOOSE },
		{ "FileHandling",                   CB_SCL_IED_Services_FileHandling },
		{ "ConfLNs",                        CB_SCL_IED_Services_ConfLNs },
		{ "SMVSettings",                    CB_SCL_IED_Services_SMVSettings },
		{ "SetDataSetValue",                CB_SCL_IED_Services_SetDataSetValue },
		{ "ConfLogControl",                 CB_SCL_IED_Services_ConfLogControl },
		{ "LogSettings",                    CB_SCL_IED_Services_LogSettings },
		{ "TimerActivatedControl",          CB_SCL_IED_Services_TimerActivatedControl },
		{ NULL,NULL }
	};

	CSpSclParser *pThis = (CSpSclParser*)cbParam;
	pThis->m_p_SCL_IED_Services = (stuSCL_IED_Services*)pThis->m_ConstString.CopyToConstBuffer(g_sCSpSclParserZero,sizeof(stuSCL_IED_Services));
	pThis->m_p_SCL_IED_Services->node_value = pThis->m_ConstString.CopyToConstString(sNodeValue);
	pThis->m_p_SCL_IED->m_Services.append(pThis->m_ConstString,pThis->m_p_SCL_IED_Services);

	return (stuSaxChoice*)pChoice;
}

stuSaxChoice* CSpSclParser::CB_SCL_IED_Services_DynAssociation(void* cbParam, SSaxAttribute *pAttribute,int iAttributeCount,char* sNodeValue)
{
	S_UNUSED(pAttribute);
	S_UNUSED(iAttributeCount);
	S_UNUSED(pAttribute);S_UNUSED(iAttributeCount);S_UNUSED(sNodeValue);static const stuSaxChoice pChoice[] = 
	{
		{ NULL,NULL }
	};

	CSpSclParser *pThis = (CSpSclParser*)cbParam;
	pThis->m_p_SCL_IED_Services_DynAssociation = (stuSCL_IED_Services_DynAssociation*)pThis->m_ConstString.CopyToConstBuffer(g_sCSpSclParserZero,sizeof(stuSCL_IED_Services_DynAssociation));
	pThis->m_p_SCL_IED_Services_DynAssociation->node_value = pThis->m_ConstString.CopyToConstString(sNodeValue);
	pThis->m_p_SCL_IED_Services->m_DynAssociation.append(pThis->m_ConstString,pThis->m_p_SCL_IED_Services_DynAssociation);

	return (stuSaxChoice*)pChoice;
}

stuSaxChoice* CSpSclParser::CB_SCL_IED_Services_SettingGroups(void* cbParam, SSaxAttribute *pAttribute,int iAttributeCount,char* sNodeValue)
{
	S_UNUSED(pAttribute);
	S_UNUSED(iAttributeCount);
	S_UNUSED(pAttribute);S_UNUSED(iAttributeCount);S_UNUSED(sNodeValue);static const stuSaxChoice pChoice[] = 
	{
		{ "SGEdit",                         CB_SCL_IED_Services_SettingGroups_SGEdit },
		{ "ConfSG",                         CB_SCL_IED_Services_SettingGroups_ConfSG },
		{ NULL,NULL }
	};

	CSpSclParser *pThis = (CSpSclParser*)cbParam;
	pThis->m_p_SCL_IED_Services_SettingGroups = (stuSCL_IED_Services_SettingGroups*)pThis->m_ConstString.CopyToConstBuffer(g_sCSpSclParserZero,sizeof(stuSCL_IED_Services_SettingGroups));
	pThis->m_p_SCL_IED_Services_SettingGroups->node_value = pThis->m_ConstString.CopyToConstString(sNodeValue);
	pThis->m_p_SCL_IED_Services->m_SettingGroups.append(pThis->m_ConstString,pThis->m_p_SCL_IED_Services_SettingGroups);

	return (stuSaxChoice*)pChoice;
}

stuSaxChoice* CSpSclParser::CB_SCL_IED_Services_SettingGroups_SGEdit(void* cbParam, SSaxAttribute *pAttribute,int iAttributeCount,char* sNodeValue)
{
	S_UNUSED(pAttribute);
	S_UNUSED(iAttributeCount);
	S_UNUSED(pAttribute);S_UNUSED(iAttributeCount);S_UNUSED(sNodeValue);static const stuSaxChoice pChoice[] = 
	{
		{ NULL,NULL }
	};

	CSpSclParser *pThis = (CSpSclParser*)cbParam;
	pThis->m_p_SCL_IED_Services_SettingGroups_SGEdit = (stuSCL_IED_Services_SettingGroups_SGEdit*)pThis->m_ConstString.CopyToConstBuffer(g_sCSpSclParserZero,sizeof(stuSCL_IED_Services_SettingGroups_SGEdit));
	pThis->m_p_SCL_IED_Services_SettingGroups_SGEdit->node_value = pThis->m_ConstString.CopyToConstString(sNodeValue);
	pThis->m_p_SCL_IED_Services_SettingGroups->m_SGEdit.append(pThis->m_ConstString,pThis->m_p_SCL_IED_Services_SettingGroups_SGEdit);

	return (stuSaxChoice*)pChoice;
}

stuSaxChoice* CSpSclParser::CB_SCL_IED_Services_SettingGroups_ConfSG(void* cbParam, SSaxAttribute *pAttribute,int iAttributeCount,char* sNodeValue)
{
	S_UNUSED(pAttribute);
	S_UNUSED(iAttributeCount);
	S_UNUSED(pAttribute);S_UNUSED(iAttributeCount);S_UNUSED(sNodeValue);static const stuSaxChoice pChoice[] = 
	{
		{ NULL,NULL }
	};

	CSpSclParser *pThis = (CSpSclParser*)cbParam;
	pThis->m_p_SCL_IED_Services_SettingGroups_ConfSG = (stuSCL_IED_Services_SettingGroups_ConfSG*)pThis->m_ConstString.CopyToConstBuffer(g_sCSpSclParserZero,sizeof(stuSCL_IED_Services_SettingGroups_ConfSG));
	pThis->m_p_SCL_IED_Services_SettingGroups_ConfSG->node_value = pThis->m_ConstString.CopyToConstString(sNodeValue);
	pThis->m_p_SCL_IED_Services_SettingGroups->m_ConfSG.append(pThis->m_ConstString,pThis->m_p_SCL_IED_Services_SettingGroups_ConfSG);

	return (stuSaxChoice*)pChoice;
}

stuSaxChoice* CSpSclParser::CB_SCL_IED_Services_GetDirectory(void* cbParam, SSaxAttribute *pAttribute,int iAttributeCount,char* sNodeValue)
{
	S_UNUSED(pAttribute);
	S_UNUSED(iAttributeCount);
	S_UNUSED(pAttribute);S_UNUSED(iAttributeCount);S_UNUSED(sNodeValue);static const stuSaxChoice pChoice[] = 
	{
		{ NULL,NULL }
	};

	CSpSclParser *pThis = (CSpSclParser*)cbParam;
	pThis->m_p_SCL_IED_Services_GetDirectory = (stuSCL_IED_Services_GetDirectory*)pThis->m_ConstString.CopyToConstBuffer(g_sCSpSclParserZero,sizeof(stuSCL_IED_Services_GetDirectory));
	pThis->m_p_SCL_IED_Services_GetDirectory->node_value = pThis->m_ConstString.CopyToConstString(sNodeValue);
	pThis->m_p_SCL_IED_Services->m_GetDirectory.append(pThis->m_ConstString,pThis->m_p_SCL_IED_Services_GetDirectory);

	return (stuSaxChoice*)pChoice;
}

stuSaxChoice* CSpSclParser::CB_SCL_IED_Services_GetDataObjectDefinition(void* cbParam, SSaxAttribute *pAttribute,int iAttributeCount,char* sNodeValue)
{
	S_UNUSED(pAttribute);
	S_UNUSED(iAttributeCount);
	S_UNUSED(pAttribute);S_UNUSED(iAttributeCount);S_UNUSED(sNodeValue);static const stuSaxChoice pChoice[] = 
	{
		{ NULL,NULL }
	};

	CSpSclParser *pThis = (CSpSclParser*)cbParam;
	pThis->m_p_SCL_IED_Services_GetDataObjectDefinition = (stuSCL_IED_Services_GetDataObjectDefinition*)pThis->m_ConstString.CopyToConstBuffer(g_sCSpSclParserZero,sizeof(stuSCL_IED_Services_GetDataObjectDefinition));
	pThis->m_p_SCL_IED_Services_GetDataObjectDefinition->node_value = pThis->m_ConstString.CopyToConstString(sNodeValue);
	pThis->m_p_SCL_IED_Services->m_GetDataObjectDefinition.append(pThis->m_ConstString,pThis->m_p_SCL_IED_Services_GetDataObjectDefinition);

	return (stuSaxChoice*)pChoice;
}

stuSaxChoice* CSpSclParser::CB_SCL_IED_Services_DataObjectDirectory(void* cbParam, SSaxAttribute *pAttribute,int iAttributeCount,char* sNodeValue)
{
	S_UNUSED(pAttribute);
	S_UNUSED(iAttributeCount);
	S_UNUSED(pAttribute);S_UNUSED(iAttributeCount);S_UNUSED(sNodeValue);static const stuSaxChoice pChoice[] = 
	{
		{ NULL,NULL }
	};

	CSpSclParser *pThis = (CSpSclParser*)cbParam;
	pThis->m_p_SCL_IED_Services_DataObjectDirectory = (stuSCL_IED_Services_DataObjectDirectory*)pThis->m_ConstString.CopyToConstBuffer(g_sCSpSclParserZero,sizeof(stuSCL_IED_Services_DataObjectDirectory));
	pThis->m_p_SCL_IED_Services_DataObjectDirectory->node_value = pThis->m_ConstString.CopyToConstString(sNodeValue);
	pThis->m_p_SCL_IED_Services->m_DataObjectDirectory.append(pThis->m_ConstString,pThis->m_p_SCL_IED_Services_DataObjectDirectory);

	return (stuSaxChoice*)pChoice;
}

stuSaxChoice* CSpSclParser::CB_SCL_IED_Services_GetDataSetValue(void* cbParam, SSaxAttribute *pAttribute,int iAttributeCount,char* sNodeValue)
{
	S_UNUSED(pAttribute);
	S_UNUSED(iAttributeCount);
	S_UNUSED(pAttribute);S_UNUSED(iAttributeCount);S_UNUSED(sNodeValue);static const stuSaxChoice pChoice[] = 
	{
		{ NULL,NULL }
	};

	CSpSclParser *pThis = (CSpSclParser*)cbParam;
	pThis->m_p_SCL_IED_Services_GetDataSetValue = (stuSCL_IED_Services_GetDataSetValue*)pThis->m_ConstString.CopyToConstBuffer(g_sCSpSclParserZero,sizeof(stuSCL_IED_Services_GetDataSetValue));
	pThis->m_p_SCL_IED_Services_GetDataSetValue->node_value = pThis->m_ConstString.CopyToConstString(sNodeValue);
	pThis->m_p_SCL_IED_Services->m_GetDataSetValue.append(pThis->m_ConstString,pThis->m_p_SCL_IED_Services_GetDataSetValue);

	return (stuSaxChoice*)pChoice;
}

stuSaxChoice* CSpSclParser::CB_SCL_IED_Services_DataSetDirectory(void* cbParam, SSaxAttribute *pAttribute,int iAttributeCount,char* sNodeValue)
{
	S_UNUSED(pAttribute);
	S_UNUSED(iAttributeCount);
	S_UNUSED(pAttribute);S_UNUSED(iAttributeCount);S_UNUSED(sNodeValue);static const stuSaxChoice pChoice[] = 
	{
		{ NULL,NULL }
	};

	CSpSclParser *pThis = (CSpSclParser*)cbParam;
	pThis->m_p_SCL_IED_Services_DataSetDirectory = (stuSCL_IED_Services_DataSetDirectory*)pThis->m_ConstString.CopyToConstBuffer(g_sCSpSclParserZero,sizeof(stuSCL_IED_Services_DataSetDirectory));
	pThis->m_p_SCL_IED_Services_DataSetDirectory->node_value = pThis->m_ConstString.CopyToConstString(sNodeValue);
	pThis->m_p_SCL_IED_Services->m_DataSetDirectory.append(pThis->m_ConstString,pThis->m_p_SCL_IED_Services_DataSetDirectory);

	return (stuSaxChoice*)pChoice;
}

stuSaxChoice* CSpSclParser::CB_SCL_IED_Services_ConfDataSet(void* cbParam, SSaxAttribute *pAttribute,int iAttributeCount,char* sNodeValue)
{
	S_UNUSED(pAttribute);
	S_UNUSED(iAttributeCount);
	S_UNUSED(pAttribute);S_UNUSED(iAttributeCount);S_UNUSED(sNodeValue);static const stuSaxChoice pChoice[] = 
	{
		{ NULL,NULL }
	};

	CSpSclParser *pThis = (CSpSclParser*)cbParam;
	pThis->m_p_SCL_IED_Services_ConfDataSet = (stuSCL_IED_Services_ConfDataSet*)pThis->m_ConstString.CopyToConstBuffer(g_sCSpSclParserZero,sizeof(stuSCL_IED_Services_ConfDataSet));
	for(int i=0;i<iAttributeCount;i++,pAttribute++)
	{
		if(SString::equals(pAttribute->m_sAttrName.data(), "max"))
			pThis->m_p_SCL_IED_Services_ConfDataSet->max = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
		else if(SString::equals(pAttribute->m_sAttrName.data(), "maxAttributes"))
			pThis->m_p_SCL_IED_Services_ConfDataSet->maxAttributes = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
	}
	pThis->m_p_SCL_IED_Services_ConfDataSet->node_value = pThis->m_ConstString.CopyToConstString(sNodeValue);
	pThis->m_p_SCL_IED_Services->m_ConfDataSet.append(pThis->m_ConstString,pThis->m_p_SCL_IED_Services_ConfDataSet);

	return (stuSaxChoice*)pChoice;
}

stuSaxChoice* CSpSclParser::CB_SCL_IED_Services_ReadWrite(void* cbParam, SSaxAttribute *pAttribute,int iAttributeCount,char* sNodeValue)
{
	S_UNUSED(pAttribute);
	S_UNUSED(iAttributeCount);
	S_UNUSED(pAttribute);S_UNUSED(iAttributeCount);S_UNUSED(sNodeValue);static const stuSaxChoice pChoice[] = 
	{
		{ NULL,NULL }
	};

	CSpSclParser *pThis = (CSpSclParser*)cbParam;
	pThis->m_p_SCL_IED_Services_ReadWrite = (stuSCL_IED_Services_ReadWrite*)pThis->m_ConstString.CopyToConstBuffer(g_sCSpSclParserZero,sizeof(stuSCL_IED_Services_ReadWrite));
	pThis->m_p_SCL_IED_Services_ReadWrite->node_value = pThis->m_ConstString.CopyToConstString(sNodeValue);
	pThis->m_p_SCL_IED_Services->m_ReadWrite.append(pThis->m_ConstString,pThis->m_p_SCL_IED_Services_ReadWrite);

	return (stuSaxChoice*)pChoice;
}

stuSaxChoice* CSpSclParser::CB_SCL_IED_Services_ConfReportControl(void* cbParam, SSaxAttribute *pAttribute,int iAttributeCount,char* sNodeValue)
{
	S_UNUSED(pAttribute);
	S_UNUSED(iAttributeCount);
	S_UNUSED(pAttribute);S_UNUSED(iAttributeCount);S_UNUSED(sNodeValue);static const stuSaxChoice pChoice[] = 
	{
		{ NULL,NULL }
	};

	CSpSclParser *pThis = (CSpSclParser*)cbParam;
	pThis->m_p_SCL_IED_Services_ConfReportControl = (stuSCL_IED_Services_ConfReportControl*)pThis->m_ConstString.CopyToConstBuffer(g_sCSpSclParserZero,sizeof(stuSCL_IED_Services_ConfReportControl));
	for(int i=0;i<iAttributeCount;i++,pAttribute++)
	{
		if(SString::equals(pAttribute->m_sAttrName.data(), "max"))
			pThis->m_p_SCL_IED_Services_ConfReportControl->max = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
	}
	pThis->m_p_SCL_IED_Services_ConfReportControl->node_value = pThis->m_ConstString.CopyToConstString(sNodeValue);
	pThis->m_p_SCL_IED_Services->m_ConfReportControl.append(pThis->m_ConstString,pThis->m_p_SCL_IED_Services_ConfReportControl);

	return (stuSaxChoice*)pChoice;
}

stuSaxChoice* CSpSclParser::CB_SCL_IED_Services_GetCBValues(void* cbParam, SSaxAttribute *pAttribute,int iAttributeCount,char* sNodeValue)
{
	S_UNUSED(pAttribute);
	S_UNUSED(iAttributeCount);
	S_UNUSED(pAttribute);S_UNUSED(iAttributeCount);S_UNUSED(sNodeValue);static const stuSaxChoice pChoice[] = 
	{
		{ NULL,NULL }
	};

	CSpSclParser *pThis = (CSpSclParser*)cbParam;
	pThis->m_p_SCL_IED_Services_GetCBValues = (stuSCL_IED_Services_GetCBValues*)pThis->m_ConstString.CopyToConstBuffer(g_sCSpSclParserZero,sizeof(stuSCL_IED_Services_GetCBValues));
	pThis->m_p_SCL_IED_Services_GetCBValues->node_value = pThis->m_ConstString.CopyToConstString(sNodeValue);
	pThis->m_p_SCL_IED_Services->m_GetCBValues.append(pThis->m_ConstString,pThis->m_p_SCL_IED_Services_GetCBValues);

	return (stuSaxChoice*)pChoice;
}

stuSaxChoice* CSpSclParser::CB_SCL_IED_Services_ReportSettings(void* cbParam, SSaxAttribute *pAttribute,int iAttributeCount,char* sNodeValue)
{
	S_UNUSED(pAttribute);
	S_UNUSED(iAttributeCount);
	S_UNUSED(pAttribute);S_UNUSED(iAttributeCount);S_UNUSED(sNodeValue);static const stuSaxChoice pChoice[] = 
	{
		{ NULL,NULL }
	};

	CSpSclParser *pThis = (CSpSclParser*)cbParam;
	pThis->m_p_SCL_IED_Services_ReportSettings = (stuSCL_IED_Services_ReportSettings*)pThis->m_ConstString.CopyToConstBuffer(g_sCSpSclParserZero,sizeof(stuSCL_IED_Services_ReportSettings));
	for(int i=0;i<iAttributeCount;i++,pAttribute++)
	{
		if(SString::equals(pAttribute->m_sAttrName.data(), "cbName"))
			pThis->m_p_SCL_IED_Services_ReportSettings->cbName = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
		else if(SString::equals(pAttribute->m_sAttrName.data(), "datSet"))
			pThis->m_p_SCL_IED_Services_ReportSettings->datSet = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
		else if(SString::equals(pAttribute->m_sAttrName.data(), "rptID"))
			pThis->m_p_SCL_IED_Services_ReportSettings->rptID = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
		else if(SString::equals(pAttribute->m_sAttrName.data(), "optFields"))
			pThis->m_p_SCL_IED_Services_ReportSettings->optFields = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
		else if(SString::equals(pAttribute->m_sAttrName.data(), "bufTime"))
			pThis->m_p_SCL_IED_Services_ReportSettings->bufTime = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
		else if(SString::equals(pAttribute->m_sAttrName.data(), "trgOps"))
			pThis->m_p_SCL_IED_Services_ReportSettings->trgOps = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
		else if(SString::equals(pAttribute->m_sAttrName.data(), "intgPd"))
			pThis->m_p_SCL_IED_Services_ReportSettings->intgPd = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
	}
	pThis->m_p_SCL_IED_Services_ReportSettings->node_value = pThis->m_ConstString.CopyToConstString(sNodeValue);
	pThis->m_p_SCL_IED_Services->m_ReportSettings.append(pThis->m_ConstString,pThis->m_p_SCL_IED_Services_ReportSettings);

	return (stuSaxChoice*)pChoice;
}

stuSaxChoice* CSpSclParser::CB_SCL_IED_Services_GSESettings(void* cbParam, SSaxAttribute *pAttribute,int iAttributeCount,char* sNodeValue)
{
	S_UNUSED(pAttribute);
	S_UNUSED(iAttributeCount);
	S_UNUSED(pAttribute);S_UNUSED(iAttributeCount);S_UNUSED(sNodeValue);static const stuSaxChoice pChoice[] = 
	{
		{ NULL,NULL }
	};

	CSpSclParser *pThis = (CSpSclParser*)cbParam;
	pThis->m_p_SCL_IED_Services_GSESettings = (stuSCL_IED_Services_GSESettings*)pThis->m_ConstString.CopyToConstBuffer(g_sCSpSclParserZero,sizeof(stuSCL_IED_Services_GSESettings));
	for(int i=0;i<iAttributeCount;i++,pAttribute++)
	{
		if(SString::equals(pAttribute->m_sAttrName.data(), "cbName"))
			pThis->m_p_SCL_IED_Services_GSESettings->cbName = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
		else if(SString::equals(pAttribute->m_sAttrName.data(), "datSet"))
			pThis->m_p_SCL_IED_Services_GSESettings->datSet = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
		else if(SString::equals(pAttribute->m_sAttrName.data(), "appID"))
			pThis->m_p_SCL_IED_Services_GSESettings->appID = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
		else if(SString::equals(pAttribute->m_sAttrName.data(), "dataLabel"))
			pThis->m_p_SCL_IED_Services_GSESettings->dataLabel = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
	}
	pThis->m_p_SCL_IED_Services_GSESettings->node_value = pThis->m_ConstString.CopyToConstString(sNodeValue);
	pThis->m_p_SCL_IED_Services->m_GSESettings.append(pThis->m_ConstString,pThis->m_p_SCL_IED_Services_GSESettings);

	return (stuSaxChoice*)pChoice;
}

stuSaxChoice* CSpSclParser::CB_SCL_IED_Services_GOOSE(void* cbParam, SSaxAttribute *pAttribute,int iAttributeCount,char* sNodeValue)
{
	S_UNUSED(pAttribute);
	S_UNUSED(iAttributeCount);
	S_UNUSED(pAttribute);S_UNUSED(iAttributeCount);S_UNUSED(sNodeValue);static const stuSaxChoice pChoice[] = 
	{
		{ NULL,NULL }
	};

	CSpSclParser *pThis = (CSpSclParser*)cbParam;
	pThis->m_p_SCL_IED_Services_GOOSE = (stuSCL_IED_Services_GOOSE*)pThis->m_ConstString.CopyToConstBuffer(g_sCSpSclParserZero,sizeof(stuSCL_IED_Services_GOOSE));
	for(int i=0;i<iAttributeCount;i++,pAttribute++)
	{
		if(SString::equals(pAttribute->m_sAttrName.data(), "max"))
			pThis->m_p_SCL_IED_Services_GOOSE->max = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
	}
	pThis->m_p_SCL_IED_Services_GOOSE->node_value = pThis->m_ConstString.CopyToConstString(sNodeValue);
	pThis->m_p_SCL_IED_Services->m_GOOSE.append(pThis->m_ConstString,pThis->m_p_SCL_IED_Services_GOOSE);

	return (stuSaxChoice*)pChoice;
}

stuSaxChoice* CSpSclParser::CB_SCL_IED_Services_FileHandling(void* cbParam, SSaxAttribute *pAttribute,int iAttributeCount,char* sNodeValue)
{
	S_UNUSED(pAttribute);
	S_UNUSED(iAttributeCount);
	S_UNUSED(pAttribute);S_UNUSED(iAttributeCount);S_UNUSED(sNodeValue);static const stuSaxChoice pChoice[] = 
	{
		{ NULL,NULL }
	};

	CSpSclParser *pThis = (CSpSclParser*)cbParam;
	pThis->m_p_SCL_IED_Services_FileHandling = (stuSCL_IED_Services_FileHandling*)pThis->m_ConstString.CopyToConstBuffer(g_sCSpSclParserZero,sizeof(stuSCL_IED_Services_FileHandling));
	pThis->m_p_SCL_IED_Services_FileHandling->node_value = pThis->m_ConstString.CopyToConstString(sNodeValue);
	pThis->m_p_SCL_IED_Services->m_FileHandling.append(pThis->m_ConstString,pThis->m_p_SCL_IED_Services_FileHandling);

	return (stuSaxChoice*)pChoice;
}

stuSaxChoice* CSpSclParser::CB_SCL_IED_Services_ConfLNs(void* cbParam, SSaxAttribute *pAttribute,int iAttributeCount,char* sNodeValue)
{
	S_UNUSED(pAttribute);
	S_UNUSED(iAttributeCount);
	S_UNUSED(pAttribute);S_UNUSED(iAttributeCount);S_UNUSED(sNodeValue);static const stuSaxChoice pChoice[] = 
	{
		{ NULL,NULL }
	};

	CSpSclParser *pThis = (CSpSclParser*)cbParam;
	pThis->m_p_SCL_IED_Services_ConfLNs = (stuSCL_IED_Services_ConfLNs*)pThis->m_ConstString.CopyToConstBuffer(g_sCSpSclParserZero,sizeof(stuSCL_IED_Services_ConfLNs));
	for(int i=0;i<iAttributeCount;i++,pAttribute++)
	{
		if(SString::equals(pAttribute->m_sAttrName.data(), "fixLnInst"))
			pThis->m_p_SCL_IED_Services_ConfLNs->fixLnInst = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
		else if(SString::equals(pAttribute->m_sAttrName.data(), "fixPrefix"))
			pThis->m_p_SCL_IED_Services_ConfLNs->fixPrefix = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
	}
	pThis->m_p_SCL_IED_Services_ConfLNs->node_value = pThis->m_ConstString.CopyToConstString(sNodeValue);
	pThis->m_p_SCL_IED_Services->m_ConfLNs.append(pThis->m_ConstString,pThis->m_p_SCL_IED_Services_ConfLNs);

	return (stuSaxChoice*)pChoice;
}

stuSaxChoice* CSpSclParser::CB_SCL_IED_Services_SMVSettings(void* cbParam, SSaxAttribute *pAttribute,int iAttributeCount,char* sNodeValue)
{
	S_UNUSED(pAttribute);
	S_UNUSED(iAttributeCount);
	S_UNUSED(pAttribute);S_UNUSED(iAttributeCount);S_UNUSED(sNodeValue);static const stuSaxChoice pChoice[] = 
	{
		{ "SmpRate",                        CB_SCL_IED_Services_SMVSettings_SmpRate },
		{ NULL,NULL }
	};

	CSpSclParser *pThis = (CSpSclParser*)cbParam;
	pThis->m_p_SCL_IED_Services_SMVSettings = (stuSCL_IED_Services_SMVSettings*)pThis->m_ConstString.CopyToConstBuffer(g_sCSpSclParserZero,sizeof(stuSCL_IED_Services_SMVSettings));
	for(int i=0;i<iAttributeCount;i++,pAttribute++)
	{
		if(SString::equals(pAttribute->m_sAttrName.data(), "cbName"))
			pThis->m_p_SCL_IED_Services_SMVSettings->cbName = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
		else if(SString::equals(pAttribute->m_sAttrName.data(), "datSet"))
			pThis->m_p_SCL_IED_Services_SMVSettings->datSet = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
		else if(SString::equals(pAttribute->m_sAttrName.data(), "optFields"))
			pThis->m_p_SCL_IED_Services_SMVSettings->optFields = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
		else if(SString::equals(pAttribute->m_sAttrName.data(), "smpRate"))
			pThis->m_p_SCL_IED_Services_SMVSettings->smpRate = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
		else if(SString::equals(pAttribute->m_sAttrName.data(), "svID"))
			pThis->m_p_SCL_IED_Services_SMVSettings->svID = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
	}
	pThis->m_p_SCL_IED_Services_SMVSettings->node_value = pThis->m_ConstString.CopyToConstString(sNodeValue);
	pThis->m_p_SCL_IED_Services->m_SMVSettings.append(pThis->m_ConstString,pThis->m_p_SCL_IED_Services_SMVSettings);

	return (stuSaxChoice*)pChoice;
}

stuSaxChoice* CSpSclParser::CB_SCL_IED_Services_SMVSettings_SmpRate(void* cbParam, SSaxAttribute *pAttribute,int iAttributeCount,char* sNodeValue)
{
	S_UNUSED(pAttribute);
	S_UNUSED(iAttributeCount);
	S_UNUSED(pAttribute);S_UNUSED(iAttributeCount);S_UNUSED(sNodeValue);static const stuSaxChoice pChoice[] = 
	{
		{ NULL,NULL }
	};

	CSpSclParser *pThis = (CSpSclParser*)cbParam;
	pThis->m_p_SCL_IED_Services_SMVSettings_SmpRate = (stuSCL_IED_Services_SMVSettings_SmpRate*)pThis->m_ConstString.CopyToConstBuffer(g_sCSpSclParserZero,sizeof(stuSCL_IED_Services_SMVSettings_SmpRate));
	pThis->m_p_SCL_IED_Services_SMVSettings_SmpRate->node_value = pThis->m_ConstString.CopyToConstString(sNodeValue);
	pThis->m_p_SCL_IED_Services_SMVSettings->m_SmpRate.append(pThis->m_ConstString,pThis->m_p_SCL_IED_Services_SMVSettings_SmpRate);

	return (stuSaxChoice*)pChoice;
}

stuSaxChoice* CSpSclParser::CB_SCL_IED_Services_SetDataSetValue(void* cbParam, SSaxAttribute *pAttribute,int iAttributeCount,char* sNodeValue)
{
	S_UNUSED(pAttribute);
	S_UNUSED(iAttributeCount);
	S_UNUSED(pAttribute);S_UNUSED(iAttributeCount);S_UNUSED(sNodeValue);static const stuSaxChoice pChoice[] = 
	{
		{ NULL,NULL }
	};

	CSpSclParser *pThis = (CSpSclParser*)cbParam;
	pThis->m_p_SCL_IED_Services_SetDataSetValue = (stuSCL_IED_Services_SetDataSetValue*)pThis->m_ConstString.CopyToConstBuffer(g_sCSpSclParserZero,sizeof(stuSCL_IED_Services_SetDataSetValue));
	pThis->m_p_SCL_IED_Services_SetDataSetValue->node_value = pThis->m_ConstString.CopyToConstString(sNodeValue);
	pThis->m_p_SCL_IED_Services->m_SetDataSetValue.append(pThis->m_ConstString,pThis->m_p_SCL_IED_Services_SetDataSetValue);

	return (stuSaxChoice*)pChoice;
}

stuSaxChoice* CSpSclParser::CB_SCL_IED_Services_ConfLogControl(void* cbParam, SSaxAttribute *pAttribute,int iAttributeCount,char* sNodeValue)
{
	S_UNUSED(pAttribute);
	S_UNUSED(iAttributeCount);
	S_UNUSED(pAttribute);S_UNUSED(iAttributeCount);S_UNUSED(sNodeValue);static const stuSaxChoice pChoice[] = 
	{
		{ NULL,NULL }
	};

	CSpSclParser *pThis = (CSpSclParser*)cbParam;
	pThis->m_p_SCL_IED_Services_ConfLogControl = (stuSCL_IED_Services_ConfLogControl*)pThis->m_ConstString.CopyToConstBuffer(g_sCSpSclParserZero,sizeof(stuSCL_IED_Services_ConfLogControl));
	for(int i=0;i<iAttributeCount;i++,pAttribute++)
	{
		if(SString::equals(pAttribute->m_sAttrName.data(), "max"))
			pThis->m_p_SCL_IED_Services_ConfLogControl->max = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
	}
	pThis->m_p_SCL_IED_Services_ConfLogControl->node_value = pThis->m_ConstString.CopyToConstString(sNodeValue);
	pThis->m_p_SCL_IED_Services->m_ConfLogControl.append(pThis->m_ConstString,pThis->m_p_SCL_IED_Services_ConfLogControl);

	return (stuSaxChoice*)pChoice;
}

stuSaxChoice* CSpSclParser::CB_SCL_IED_Services_LogSettings(void* cbParam, SSaxAttribute *pAttribute,int iAttributeCount,char* sNodeValue)
{
	S_UNUSED(pAttribute);
	S_UNUSED(iAttributeCount);
	S_UNUSED(pAttribute);S_UNUSED(iAttributeCount);S_UNUSED(sNodeValue);static const stuSaxChoice pChoice[] = 
	{
		{ NULL,NULL }
	};

	CSpSclParser *pThis = (CSpSclParser*)cbParam;
	pThis->m_p_SCL_IED_Services_LogSettings = (stuSCL_IED_Services_LogSettings*)pThis->m_ConstString.CopyToConstBuffer(g_sCSpSclParserZero,sizeof(stuSCL_IED_Services_LogSettings));
	for(int i=0;i<iAttributeCount;i++,pAttribute++)
	{
		if(SString::equals(pAttribute->m_sAttrName.data(), "cbName"))
			pThis->m_p_SCL_IED_Services_LogSettings->cbName = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
		else if(SString::equals(pAttribute->m_sAttrName.data(), "datSet"))
			pThis->m_p_SCL_IED_Services_LogSettings->datSet = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
		else if(SString::equals(pAttribute->m_sAttrName.data(), "intgPd"))
			pThis->m_p_SCL_IED_Services_LogSettings->intgPd = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
		else if(SString::equals(pAttribute->m_sAttrName.data(), "logEna"))
			pThis->m_p_SCL_IED_Services_LogSettings->logEna = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
		else if(SString::equals(pAttribute->m_sAttrName.data(), "trgOps"))
			pThis->m_p_SCL_IED_Services_LogSettings->trgOps = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
	}
	pThis->m_p_SCL_IED_Services_LogSettings->node_value = pThis->m_ConstString.CopyToConstString(sNodeValue);
	pThis->m_p_SCL_IED_Services->m_LogSettings.append(pThis->m_ConstString,pThis->m_p_SCL_IED_Services_LogSettings);

	return (stuSaxChoice*)pChoice;
}

stuSaxChoice* CSpSclParser::CB_SCL_IED_Services_TimerActivatedControl(void* cbParam, SSaxAttribute *pAttribute,int iAttributeCount,char* sNodeValue)
{
	S_UNUSED(pAttribute);
	S_UNUSED(iAttributeCount);
	S_UNUSED(pAttribute);S_UNUSED(iAttributeCount);S_UNUSED(sNodeValue);static const stuSaxChoice pChoice[] = 
	{
		{ NULL,NULL }
	};

	CSpSclParser *pThis = (CSpSclParser*)cbParam;
	pThis->m_p_SCL_IED_Services_TimerActivatedControl = (stuSCL_IED_Services_TimerActivatedControl*)pThis->m_ConstString.CopyToConstBuffer(g_sCSpSclParserZero,sizeof(stuSCL_IED_Services_TimerActivatedControl));
	pThis->m_p_SCL_IED_Services_TimerActivatedControl->node_value = pThis->m_ConstString.CopyToConstString(sNodeValue);
	pThis->m_p_SCL_IED_Services->m_TimerActivatedControl.append(pThis->m_ConstString,pThis->m_p_SCL_IED_Services_TimerActivatedControl);

	return (stuSaxChoice*)pChoice;
}

stuSaxChoice* CSpSclParser::CB_SCL_IED_AccessPoint(void* cbParam, SSaxAttribute *pAttribute,int iAttributeCount,char* sNodeValue)
{
	S_UNUSED(pAttribute);
	S_UNUSED(iAttributeCount);
	S_UNUSED(pAttribute);S_UNUSED(iAttributeCount);S_UNUSED(sNodeValue);static const stuSaxChoice pChoice[] = 
	{
		{ "Server",                         CB_SCL_IED_AccessPoint_Server },
		{ NULL,NULL }
	};

	CSpSclParser *pThis = (CSpSclParser*)cbParam;
	pThis->m_p_SCL_IED_AccessPoint = (stuSCL_IED_AccessPoint*)pThis->m_ConstString.CopyToConstBuffer(g_sCSpSclParserZero,sizeof(stuSCL_IED_AccessPoint));
	for(int i=0;i<iAttributeCount;i++,pAttribute++)
	{
		if(SString::equals(pAttribute->m_sAttrName.data(), "name"))
			pThis->m_p_SCL_IED_AccessPoint->name = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
		else if(SString::equals(pAttribute->m_sAttrName.data(), "router"))
			pThis->m_p_SCL_IED_AccessPoint->router = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
		else if(SString::equals(pAttribute->m_sAttrName.data(), "clock"))
			pThis->m_p_SCL_IED_AccessPoint->clock = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
		else if(SString::equals(pAttribute->m_sAttrName.data(), "desc"))
			pThis->m_p_SCL_IED_AccessPoint->desc = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
	}
	pThis->m_p_SCL_IED_AccessPoint->node_value = pThis->m_ConstString.CopyToConstString(sNodeValue);
	pThis->m_p_SCL_IED->m_AccessPoint.append(pThis->m_ConstString,pThis->m_p_SCL_IED_AccessPoint);

	return (stuSaxChoice*)pChoice;
}

stuSaxChoice* CSpSclParser::CB_SCL_IED_AccessPoint_Server(void* cbParam, SSaxAttribute *pAttribute,int iAttributeCount,char* sNodeValue)
{
	S_UNUSED(pAttribute);
	S_UNUSED(iAttributeCount);
	S_UNUSED(pAttribute);S_UNUSED(iAttributeCount);S_UNUSED(sNodeValue);static const stuSaxChoice pChoice[] = 
	{
		{ "Authentication",                 CB_SCL_IED_AccessPoint_Server_Authentication },
		{ "LDevice",                        CB_SCL_IED_AccessPoint_Server_LDevice },
		{ NULL,NULL }
	};

	CSpSclParser *pThis = (CSpSclParser*)cbParam;
	pThis->m_p_SCL_IED_AccessPoint_Server = (stuSCL_IED_AccessPoint_Server*)pThis->m_ConstString.CopyToConstBuffer(g_sCSpSclParserZero,sizeof(stuSCL_IED_AccessPoint_Server));
	for(int i=0;i<iAttributeCount;i++,pAttribute++)
	{
		if(SString::equals(pAttribute->m_sAttrName.data(), "timeout"))
			pThis->m_p_SCL_IED_AccessPoint_Server->timeout = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
		else if(SString::equals(pAttribute->m_sAttrName.data(), "desc"))
			pThis->m_p_SCL_IED_AccessPoint_Server->desc = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
	}
	pThis->m_p_SCL_IED_AccessPoint_Server->node_value = pThis->m_ConstString.CopyToConstString(sNodeValue);
	pThis->m_p_SCL_IED_AccessPoint->m_Server.append(pThis->m_ConstString,pThis->m_p_SCL_IED_AccessPoint_Server);

	return (stuSaxChoice*)pChoice;
}

stuSaxChoice* CSpSclParser::CB_SCL_IED_AccessPoint_Server_Authentication(void* cbParam, SSaxAttribute *pAttribute,int iAttributeCount,char* sNodeValue)
{
	S_UNUSED(pAttribute);
	S_UNUSED(iAttributeCount);
	S_UNUSED(pAttribute);S_UNUSED(iAttributeCount);S_UNUSED(sNodeValue);static const stuSaxChoice pChoice[] = 
	{
		{ NULL,NULL }
	};

	CSpSclParser *pThis = (CSpSclParser*)cbParam;
	pThis->m_p_SCL_IED_AccessPoint_Server_Authentication = (stuSCL_IED_AccessPoint_Server_Authentication*)pThis->m_ConstString.CopyToConstBuffer(g_sCSpSclParserZero,sizeof(stuSCL_IED_AccessPoint_Server_Authentication));
	for(int i=0;i<iAttributeCount;i++,pAttribute++)
	{
		if(SString::equals(pAttribute->m_sAttrName.data(), "none"))
			pThis->m_p_SCL_IED_AccessPoint_Server_Authentication->none = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
		else if(SString::equals(pAttribute->m_sAttrName.data(), "certificate"))
			pThis->m_p_SCL_IED_AccessPoint_Server_Authentication->certificate = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
		else if(SString::equals(pAttribute->m_sAttrName.data(), "strong"))
			pThis->m_p_SCL_IED_AccessPoint_Server_Authentication->strong = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
		else if(SString::equals(pAttribute->m_sAttrName.data(), "weak"))
			pThis->m_p_SCL_IED_AccessPoint_Server_Authentication->weak = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
		else if(SString::equals(pAttribute->m_sAttrName.data(), "password"))
			pThis->m_p_SCL_IED_AccessPoint_Server_Authentication->password = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
	}
	pThis->m_p_SCL_IED_AccessPoint_Server_Authentication->node_value = pThis->m_ConstString.CopyToConstString(sNodeValue);
	pThis->m_p_SCL_IED_AccessPoint_Server->m_Authentication.append(pThis->m_ConstString,pThis->m_p_SCL_IED_AccessPoint_Server_Authentication);

	return (stuSaxChoice*)pChoice;
}

stuSaxChoice* CSpSclParser::CB_SCL_IED_AccessPoint_Server_LDevice(void* cbParam, SSaxAttribute *pAttribute,int iAttributeCount,char* sNodeValue)
{
	S_UNUSED(pAttribute);
	S_UNUSED(iAttributeCount);
	S_UNUSED(pAttribute);S_UNUSED(iAttributeCount);S_UNUSED(sNodeValue);static const stuSaxChoice pChoice[] = 
	{
		{ "LN0",                            CB_SCL_IED_AccessPoint_Server_LDevice_LN0 },
		{ "LN",                             CB_SCL_IED_AccessPoint_Server_LDevice_LN },
		{ NULL,NULL }
	};

	CSpSclParser *pThis = (CSpSclParser*)cbParam;
	pThis->m_p_SCL_IED_AccessPoint_Server_LDevice = (stuSCL_IED_AccessPoint_Server_LDevice*)pThis->m_ConstString.CopyToConstBuffer(g_sCSpSclParserZero,sizeof(stuSCL_IED_AccessPoint_Server_LDevice));
	for(int i=0;i<iAttributeCount;i++,pAttribute++)
	{
		if(SString::equals(pAttribute->m_sAttrName.data(), "inst"))
			pThis->m_p_SCL_IED_AccessPoint_Server_LDevice->inst = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
		else if(SString::equals(pAttribute->m_sAttrName.data(), "desc"))
			pThis->m_p_SCL_IED_AccessPoint_Server_LDevice->desc = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
	}
	pThis->m_p_SCL_IED_AccessPoint_Server_LDevice->node_value = pThis->m_ConstString.CopyToConstString(sNodeValue);
	pThis->m_p_SCL_IED_AccessPoint_Server->m_LDevice.append(pThis->m_ConstString,pThis->m_p_SCL_IED_AccessPoint_Server_LDevice);

	return (stuSaxChoice*)pChoice;
}

stuSaxChoice* CSpSclParser::CB_SCL_IED_AccessPoint_Server_LDevice_LN0(void* cbParam, SSaxAttribute *pAttribute,int iAttributeCount,char* sNodeValue)
{
	S_UNUSED(pAttribute);
	S_UNUSED(iAttributeCount);
	S_UNUSED(pAttribute);S_UNUSED(iAttributeCount);S_UNUSED(sNodeValue);static const stuSaxChoice pChoice[] = 
	{
		{ "DataSet",                        CB_SCL_IED_AccessPoint_Server_LDevice_LN0_DataSet },
		{ "ReportControl",                  CB_SCL_IED_AccessPoint_Server_LDevice_LN0_ReportControl },
		{ "DOI",                            CB_SCL_IED_AccessPoint_Server_LDevice_LN0_DOI },
		{ "SettingControl",                 CB_SCL_IED_AccessPoint_Server_LDevice_LN0_SettingControl },
		{ "Inputs",                         CB_SCL_IED_AccessPoint_Server_LDevice_LN0_Inputs },
		{ "GSEControl",                     CB_SCL_IED_AccessPoint_Server_LDevice_LN0_GSEControl },
		{ "LogControl",                     CB_SCL_IED_AccessPoint_Server_LDevice_LN0_LogControl },
		{ "SampledValueControl",            CB_SCL_IED_AccessPoint_Server_LDevice_LN0_SampledValueControl },
		{ NULL,NULL }
	};

	CSpSclParser *pThis = (CSpSclParser*)cbParam;
	pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN0 = (stuSCL_IED_AccessPoint_Server_LDevice_LN0*)pThis->m_ConstString.CopyToConstBuffer(g_sCSpSclParserZero,sizeof(stuSCL_IED_AccessPoint_Server_LDevice_LN0));
	for(int i=0;i<iAttributeCount;i++,pAttribute++)
	{
		if(SString::equals(pAttribute->m_sAttrName.data(), "lnType"))
			pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN0->lnType = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
		else if(SString::equals(pAttribute->m_sAttrName.data(), "lnClass"))
			pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN0->lnClass = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
		else if(SString::equals(pAttribute->m_sAttrName.data(), "inst"))
			pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN0->inst = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
		else if(SString::equals(pAttribute->m_sAttrName.data(), "desc"))
			pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN0->desc = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
	}
	pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN0->node_value = pThis->m_ConstString.CopyToConstString(sNodeValue);
	pThis->m_p_SCL_IED_AccessPoint_Server_LDevice->m_LN0.append(pThis->m_ConstString,pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN0);

	return (stuSaxChoice*)pChoice;
}

stuSaxChoice* CSpSclParser::CB_SCL_IED_AccessPoint_Server_LDevice_LN0_DataSet(void* cbParam, SSaxAttribute *pAttribute,int iAttributeCount,char* sNodeValue)
{
	S_UNUSED(pAttribute);
	S_UNUSED(iAttributeCount);
	S_UNUSED(pAttribute);S_UNUSED(iAttributeCount);S_UNUSED(sNodeValue);static const stuSaxChoice pChoice[] = 
	{
		{ "FCDA",                           CB_SCL_IED_AccessPoint_Server_LDevice_LN0_DataSet_FCDA },
		{ NULL,NULL }
	};

	CSpSclParser *pThis = (CSpSclParser*)cbParam;
	pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN0_DataSet = (stuSCL_IED_AccessPoint_Server_LDevice_LN0_DataSet*)pThis->m_ConstString.CopyToConstBuffer(g_sCSpSclParserZero,sizeof(stuSCL_IED_AccessPoint_Server_LDevice_LN0_DataSet));
	for(int i=0;i<iAttributeCount;i++,pAttribute++)
	{
		if(SString::equals(pAttribute->m_sAttrName.data(), "desc"))
			pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN0_DataSet->desc = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
		else if(SString::equals(pAttribute->m_sAttrName.data(), "name"))
			pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN0_DataSet->name = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
	}
	pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN0_DataSet->node_value = pThis->m_ConstString.CopyToConstString(sNodeValue);
	pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN0->m_DataSet.append(pThis->m_ConstString,pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN0_DataSet);

	return (stuSaxChoice*)pChoice;
}

stuSaxChoice* CSpSclParser::CB_SCL_IED_AccessPoint_Server_LDevice_LN0_DataSet_FCDA(void* cbParam, SSaxAttribute *pAttribute,int iAttributeCount,char* sNodeValue)
{
	S_UNUSED(pAttribute);
	S_UNUSED(iAttributeCount);
	S_UNUSED(pAttribute);S_UNUSED(iAttributeCount);S_UNUSED(sNodeValue);static const stuSaxChoice pChoice[] = 
	{
		{ NULL,NULL }
	};

	CSpSclParser *pThis = (CSpSclParser*)cbParam;
	pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN0_DataSet_FCDA = (stuSCL_IED_AccessPoint_Server_LDevice_LN0_DataSet_FCDA*)pThis->m_ConstString.CopyToConstBuffer(g_sCSpSclParserZero,sizeof(stuSCL_IED_AccessPoint_Server_LDevice_LN0_DataSet_FCDA));
	for(int i=0;i<iAttributeCount;i++,pAttribute++)
	{
		if(SString::equals(pAttribute->m_sAttrName.data(), "ldInst"))
			pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN0_DataSet_FCDA->ldInst = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
		else if(SString::equals(pAttribute->m_sAttrName.data(), "prefix"))
			pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN0_DataSet_FCDA->prefix = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
		else if(SString::equals(pAttribute->m_sAttrName.data(), "lnClass"))
			pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN0_DataSet_FCDA->lnClass = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
		else if(SString::equals(pAttribute->m_sAttrName.data(), "doName"))
			pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN0_DataSet_FCDA->doName = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
		else if(SString::equals(pAttribute->m_sAttrName.data(), "fc"))
			pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN0_DataSet_FCDA->fc = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
		else if(SString::equals(pAttribute->m_sAttrName.data(), "daName"))
			pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN0_DataSet_FCDA->daName = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
		else if(SString::equals(pAttribute->m_sAttrName.data(), "lnInst"))
			pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN0_DataSet_FCDA->lnInst = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
	}
	pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN0_DataSet_FCDA->node_value = pThis->m_ConstString.CopyToConstString(sNodeValue);
	pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN0_DataSet->m_FCDA.append(pThis->m_ConstString,pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN0_DataSet_FCDA);

	return (stuSaxChoice*)pChoice;
}

stuSaxChoice* CSpSclParser::CB_SCL_IED_AccessPoint_Server_LDevice_LN0_ReportControl(void* cbParam, SSaxAttribute *pAttribute,int iAttributeCount,char* sNodeValue)
{
	S_UNUSED(pAttribute);
	S_UNUSED(iAttributeCount);
	S_UNUSED(pAttribute);S_UNUSED(iAttributeCount);S_UNUSED(sNodeValue);static const stuSaxChoice pChoice[] = 
	{
		{ "TrgOps",                         CB_SCL_IED_AccessPoint_Server_LDevice_LN0_ReportControl_TrgOps },
		{ "OptFields",                      CB_SCL_IED_AccessPoint_Server_LDevice_LN0_ReportControl_OptFields },
		{ "RptEnabled",                     CB_SCL_IED_AccessPoint_Server_LDevice_LN0_ReportControl_RptEnabled },
		{ "Private",                        CB_SCL_IED_AccessPoint_Server_LDevice_LN0_ReportControl_Private },
		{ NULL,NULL }
	};

	CSpSclParser *pThis = (CSpSclParser*)cbParam;
	pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN0_ReportControl = (stuSCL_IED_AccessPoint_Server_LDevice_LN0_ReportControl*)pThis->m_ConstString.CopyToConstBuffer(g_sCSpSclParserZero,sizeof(stuSCL_IED_AccessPoint_Server_LDevice_LN0_ReportControl));
	for(int i=0;i<iAttributeCount;i++,pAttribute++)
	{
		if(SString::equals(pAttribute->m_sAttrName.data(), "name"))
			pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN0_ReportControl->name = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
		else if(SString::equals(pAttribute->m_sAttrName.data(), "datSet"))
			pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN0_ReportControl->datSet = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
		else if(SString::equals(pAttribute->m_sAttrName.data(), "intgPd"))
			pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN0_ReportControl->intgPd = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
		else if(SString::equals(pAttribute->m_sAttrName.data(), "rptID"))
			pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN0_ReportControl->rptID = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
		else if(SString::equals(pAttribute->m_sAttrName.data(), "confRev"))
			pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN0_ReportControl->confRev = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
		else if(SString::equals(pAttribute->m_sAttrName.data(), "buffered"))
			pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN0_ReportControl->buffered = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
		else if(SString::equals(pAttribute->m_sAttrName.data(), "bufTime"))
			pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN0_ReportControl->bufTime = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
		else if(SString::equals(pAttribute->m_sAttrName.data(), "desc"))
			pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN0_ReportControl->desc = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
	}
	pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN0_ReportControl->node_value = pThis->m_ConstString.CopyToConstString(sNodeValue);
	pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN0->m_ReportControl.append(pThis->m_ConstString,pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN0_ReportControl);

	return (stuSaxChoice*)pChoice;
}

stuSaxChoice* CSpSclParser::CB_SCL_IED_AccessPoint_Server_LDevice_LN0_ReportControl_TrgOps(void* cbParam, SSaxAttribute *pAttribute,int iAttributeCount,char* sNodeValue)
{
	S_UNUSED(pAttribute);
	S_UNUSED(iAttributeCount);
	S_UNUSED(pAttribute);S_UNUSED(iAttributeCount);S_UNUSED(sNodeValue);static const stuSaxChoice pChoice[] = 
	{
		{ NULL,NULL }
	};

	CSpSclParser *pThis = (CSpSclParser*)cbParam;
	pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN0_ReportControl_TrgOps = (stuSCL_IED_AccessPoint_Server_LDevice_LN0_ReportControl_TrgOps*)pThis->m_ConstString.CopyToConstBuffer(g_sCSpSclParserZero,sizeof(stuSCL_IED_AccessPoint_Server_LDevice_LN0_ReportControl_TrgOps));
	for(int i=0;i<iAttributeCount;i++,pAttribute++)
	{
		if(SString::equals(pAttribute->m_sAttrName.data(), "qchg"))
			pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN0_ReportControl_TrgOps->qchg = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
		else if(SString::equals(pAttribute->m_sAttrName.data(), "period"))
			pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN0_ReportControl_TrgOps->period = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
		else if(SString::equals(pAttribute->m_sAttrName.data(), "dupd"))
			pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN0_ReportControl_TrgOps->dupd = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
		else if(SString::equals(pAttribute->m_sAttrName.data(), "dchg"))
			pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN0_ReportControl_TrgOps->dchg = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
	}
	pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN0_ReportControl_TrgOps->node_value = pThis->m_ConstString.CopyToConstString(sNodeValue);
	pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN0_ReportControl->m_TrgOps.append(pThis->m_ConstString,pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN0_ReportControl_TrgOps);

	return (stuSaxChoice*)pChoice;
}

stuSaxChoice* CSpSclParser::CB_SCL_IED_AccessPoint_Server_LDevice_LN0_ReportControl_OptFields(void* cbParam, SSaxAttribute *pAttribute,int iAttributeCount,char* sNodeValue)
{
	S_UNUSED(pAttribute);
	S_UNUSED(iAttributeCount);
	S_UNUSED(pAttribute);S_UNUSED(iAttributeCount);S_UNUSED(sNodeValue);static const stuSaxChoice pChoice[] = 
	{
		{ NULL,NULL }
	};

	CSpSclParser *pThis = (CSpSclParser*)cbParam;
	pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN0_ReportControl_OptFields = (stuSCL_IED_AccessPoint_Server_LDevice_LN0_ReportControl_OptFields*)pThis->m_ConstString.CopyToConstBuffer(g_sCSpSclParserZero,sizeof(stuSCL_IED_AccessPoint_Server_LDevice_LN0_ReportControl_OptFields));
	for(int i=0;i<iAttributeCount;i++,pAttribute++)
	{
		if(SString::equals(pAttribute->m_sAttrName.data(), "dataRef"))
			pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN0_ReportControl_OptFields->dataRef = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
		else if(SString::equals(pAttribute->m_sAttrName.data(), "reasonCode"))
			pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN0_ReportControl_OptFields->reasonCode = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
		else if(SString::equals(pAttribute->m_sAttrName.data(), "configRef"))
			pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN0_ReportControl_OptFields->configRef = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
		else if(SString::equals(pAttribute->m_sAttrName.data(), "dataSet"))
			pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN0_ReportControl_OptFields->dataSet = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
		else if(SString::equals(pAttribute->m_sAttrName.data(), "entryID"))
			pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN0_ReportControl_OptFields->entryID = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
		else if(SString::equals(pAttribute->m_sAttrName.data(), "timeStamp"))
			pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN0_ReportControl_OptFields->timeStamp = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
		else if(SString::equals(pAttribute->m_sAttrName.data(), "seqNum"))
			pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN0_ReportControl_OptFields->seqNum = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
		else if(SString::equals(pAttribute->m_sAttrName.data(), "segmentation"))
			pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN0_ReportControl_OptFields->segmentation = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
		else if(SString::equals(pAttribute->m_sAttrName.data(), "bufOvfl"))
			pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN0_ReportControl_OptFields->bufOvfl = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
	}
	pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN0_ReportControl_OptFields->node_value = pThis->m_ConstString.CopyToConstString(sNodeValue);
	pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN0_ReportControl->m_OptFields.append(pThis->m_ConstString,pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN0_ReportControl_OptFields);

	return (stuSaxChoice*)pChoice;
}

stuSaxChoice* CSpSclParser::CB_SCL_IED_AccessPoint_Server_LDevice_LN0_ReportControl_RptEnabled(void* cbParam, SSaxAttribute *pAttribute,int iAttributeCount,char* sNodeValue)
{
	S_UNUSED(pAttribute);
	S_UNUSED(iAttributeCount);
	S_UNUSED(pAttribute);S_UNUSED(iAttributeCount);S_UNUSED(sNodeValue);static const stuSaxChoice pChoice[] = 
	{
		{ NULL,NULL }
	};

	CSpSclParser *pThis = (CSpSclParser*)cbParam;
	pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN0_ReportControl_RptEnabled = (stuSCL_IED_AccessPoint_Server_LDevice_LN0_ReportControl_RptEnabled*)pThis->m_ConstString.CopyToConstBuffer(g_sCSpSclParserZero,sizeof(stuSCL_IED_AccessPoint_Server_LDevice_LN0_ReportControl_RptEnabled));
	for(int i=0;i<iAttributeCount;i++,pAttribute++)
	{
		if(SString::equals(pAttribute->m_sAttrName.data(), "max"))
			pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN0_ReportControl_RptEnabled->max = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
	}
	pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN0_ReportControl_RptEnabled->node_value = pThis->m_ConstString.CopyToConstString(sNodeValue);
	pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN0_ReportControl->m_RptEnabled.append(pThis->m_ConstString,pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN0_ReportControl_RptEnabled);

	return (stuSaxChoice*)pChoice;
}

stuSaxChoice* CSpSclParser::CB_SCL_IED_AccessPoint_Server_LDevice_LN0_ReportControl_Private(void* cbParam, SSaxAttribute *pAttribute,int iAttributeCount,char* sNodeValue)
{
	S_UNUSED(pAttribute);S_UNUSED(iAttributeCount);S_UNUSED(sNodeValue);static const stuSaxChoice pChoice[] = 
	{
		{ NULL,NULL }
	};

	CSpSclParser *pThis = (CSpSclParser*)cbParam;
	pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN0_ReportControl_Private = (stuSCL_IED_AccessPoint_Server_LDevice_LN0_ReportControl_Private*)pThis->m_ConstString.CopyToConstBuffer(g_sCSpSclParserZero,sizeof(stuSCL_IED_AccessPoint_Server_LDevice_LN0_ReportControl_Private));
	for(int i=0;i<iAttributeCount;i++,pAttribute++)
	{
		if(SString::equals(pAttribute->m_sAttrName.data(), "sznari"))
			pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN0_ReportControl_Private->sznari = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
		else if(SString::equals(pAttribute->m_sAttrName.data(), "ScanRate"))
			pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN0_ReportControl_Private->ScanRate = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
	}
	pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN0_ReportControl_Private->node_value = pThis->m_ConstString.CopyToConstString(sNodeValue);
	pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN0_ReportControl->m_Private.append(pThis->m_ConstString,pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN0_ReportControl_Private);

	return (stuSaxChoice*)pChoice;
}

stuSaxChoice* CSpSclParser::CB_SCL_IED_AccessPoint_Server_LDevice_LN0_DOI(void* cbParam, SSaxAttribute *pAttribute,int iAttributeCount,char* sNodeValue)
{
	S_UNUSED(pAttribute);S_UNUSED(iAttributeCount);S_UNUSED(sNodeValue);static const stuSaxChoice pChoice[] = 
	{
		{ "DAI",                            CB_SCL_IED_AccessPoint_Server_LDevice_LN0_DOI_DAI },
		{ "SDI",                            CB_SCL_IED_AccessPoint_Server_LDevice_LN0_DOI_SDI },
		{ NULL,NULL }
	};

	CSpSclParser *pThis = (CSpSclParser*)cbParam;
	pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN0_DOI = (stuSCL_IED_AccessPoint_Server_LDevice_LN0_DOI*)pThis->m_ConstString.CopyToConstBuffer(g_sCSpSclParserZero,sizeof(stuSCL_IED_AccessPoint_Server_LDevice_LN0_DOI));
	for(int i=0;i<iAttributeCount;i++,pAttribute++)
	{
		if(SString::equals(pAttribute->m_sAttrName.data(), "name"))
			pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN0_DOI->name = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
		else if(SString::equals(pAttribute->m_sAttrName.data(), "desc"))
			pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN0_DOI->desc = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
	}
	pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN0_DOI->node_value = pThis->m_ConstString.CopyToConstString(sNodeValue);
	pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN0->m_DOI.append(pThis->m_ConstString,pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN0_DOI);

	return (stuSaxChoice*)pChoice;
}

stuSaxChoice* CSpSclParser::CB_SCL_IED_AccessPoint_Server_LDevice_LN0_DOI_DAI(void* cbParam, SSaxAttribute *pAttribute,int iAttributeCount,char* sNodeValue)
{
	S_UNUSED(pAttribute);S_UNUSED(iAttributeCount);S_UNUSED(sNodeValue);static const stuSaxChoice pChoice[] = 
	{
		{ "Val",                            CB_SCL_IED_AccessPoint_Server_LDevice_LN0_DOI_DAI_Val },
		{ NULL,NULL }
	};

	CSpSclParser *pThis = (CSpSclParser*)cbParam;
	pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN0_DOI_DAI = (stuSCL_IED_AccessPoint_Server_LDevice_LN0_DOI_DAI*)pThis->m_ConstString.CopyToConstBuffer(g_sCSpSclParserZero,sizeof(stuSCL_IED_AccessPoint_Server_LDevice_LN0_DOI_DAI));
	for(int i=0;i<iAttributeCount;i++,pAttribute++)
	{
		if(SString::equals(pAttribute->m_sAttrName.data(), "name"))
			pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN0_DOI_DAI->name = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
		else if(SString::equals(pAttribute->m_sAttrName.data(), "valKind"))
			pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN0_DOI_DAI->valKind = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
		else if(SString::equals(pAttribute->m_sAttrName.data(), "sAddr"))
			pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN0_DOI_DAI->sAddr = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
		else if(SString::equals(pAttribute->m_sAttrName.data(), "desc"))
			pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN0_DOI_DAI->desc = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
	}
	pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN0_DOI_DAI->node_value = pThis->m_ConstString.CopyToConstString(sNodeValue);
	pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN0_DOI->m_DAI.append(pThis->m_ConstString,pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN0_DOI_DAI);

	return (stuSaxChoice*)pChoice;
}

stuSaxChoice* CSpSclParser::CB_SCL_IED_AccessPoint_Server_LDevice_LN0_DOI_DAI_Val(void* cbParam, SSaxAttribute *pAttribute,int iAttributeCount,char* sNodeValue)
{
	S_UNUSED(pAttribute);S_UNUSED(iAttributeCount);S_UNUSED(sNodeValue);static const stuSaxChoice pChoice[] = 
	{
		{ NULL,NULL }
	};

	CSpSclParser *pThis = (CSpSclParser*)cbParam;
	pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN0_DOI_DAI_Val = (stuSCL_IED_AccessPoint_Server_LDevice_LN0_DOI_DAI_Val*)pThis->m_ConstString.CopyToConstBuffer(g_sCSpSclParserZero,sizeof(stuSCL_IED_AccessPoint_Server_LDevice_LN0_DOI_DAI_Val));
	pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN0_DOI_DAI_Val->node_value = pThis->m_ConstString.CopyToConstString(sNodeValue);
	pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN0_DOI_DAI->m_Val.append(pThis->m_ConstString,pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN0_DOI_DAI_Val);

	return (stuSaxChoice*)pChoice;
}

stuSaxChoice* CSpSclParser::CB_SCL_IED_AccessPoint_Server_LDevice_LN0_DOI_SDI(void* cbParam, SSaxAttribute *pAttribute,int iAttributeCount,char* sNodeValue)
{
	S_UNUSED(pAttribute);S_UNUSED(iAttributeCount);S_UNUSED(sNodeValue);static const stuSaxChoice pChoice[] = 
	{
		{ "DAI",                            CB_SCL_IED_AccessPoint_Server_LDevice_LN0_DOI_SDI_DAI },
		{ "SDI",                            CB_SCL_IED_AccessPoint_Server_LDevice_LN0_DOI_SDI_SDI },
		{ NULL,NULL }
	};

	CSpSclParser *pThis = (CSpSclParser*)cbParam;
	pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN0_DOI_SDI = (stuSCL_IED_AccessPoint_Server_LDevice_LN0_DOI_SDI*)pThis->m_ConstString.CopyToConstBuffer(g_sCSpSclParserZero,sizeof(stuSCL_IED_AccessPoint_Server_LDevice_LN0_DOI_SDI));
	for(int i=0;i<iAttributeCount;i++,pAttribute++)
	{
		if(SString::equals(pAttribute->m_sAttrName.data(), "name"))
			pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN0_DOI_SDI->name = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
	}
	pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN0_DOI_SDI->node_value = pThis->m_ConstString.CopyToConstString(sNodeValue);
	pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN0_DOI->m_SDI.append(pThis->m_ConstString,pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN0_DOI_SDI);

	return (stuSaxChoice*)pChoice;
}

stuSaxChoice* CSpSclParser::CB_SCL_IED_AccessPoint_Server_LDevice_LN0_DOI_SDI_DAI(void* cbParam, SSaxAttribute *pAttribute,int iAttributeCount,char* sNodeValue)
{
	S_UNUSED(pAttribute);S_UNUSED(iAttributeCount);S_UNUSED(sNodeValue);static const stuSaxChoice pChoice[] = 
	{
		{ "Val",                            CB_SCL_IED_AccessPoint_Server_LDevice_LN0_DOI_SDI_DAI_Val },
		{ NULL,NULL }
	};

	CSpSclParser *pThis = (CSpSclParser*)cbParam;
	pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN0_DOI_SDI_DAI = (stuSCL_IED_AccessPoint_Server_LDevice_LN0_DOI_SDI_DAI*)pThis->m_ConstString.CopyToConstBuffer(g_sCSpSclParserZero,sizeof(stuSCL_IED_AccessPoint_Server_LDevice_LN0_DOI_SDI_DAI));
	for(int i=0;i<iAttributeCount;i++,pAttribute++)
	{
		if(SString::equals(pAttribute->m_sAttrName.data(), "name"))
			pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN0_DOI_SDI_DAI->name = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
		else if(SString::equals(pAttribute->m_sAttrName.data(), "valKind"))
			pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN0_DOI_SDI_DAI->valKind = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
		else if(SString::equals(pAttribute->m_sAttrName.data(), "sAddr"))
			pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN0_DOI_SDI_DAI->sAddr = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
	}
	pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN0_DOI_SDI_DAI->node_value = pThis->m_ConstString.CopyToConstString(sNodeValue);
	pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN0_DOI_SDI->m_DAI.append(pThis->m_ConstString,pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN0_DOI_SDI_DAI);

	return (stuSaxChoice*)pChoice;
}

stuSaxChoice* CSpSclParser::CB_SCL_IED_AccessPoint_Server_LDevice_LN0_DOI_SDI_DAI_Val(void* cbParam, SSaxAttribute *pAttribute,int iAttributeCount,char* sNodeValue)
{
	S_UNUSED(pAttribute);S_UNUSED(iAttributeCount);S_UNUSED(sNodeValue);static const stuSaxChoice pChoice[] = 
	{
		{ NULL,NULL }
	};

	CSpSclParser *pThis = (CSpSclParser*)cbParam;
	pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN0_DOI_SDI_DAI_Val = (stuSCL_IED_AccessPoint_Server_LDevice_LN0_DOI_SDI_DAI_Val*)pThis->m_ConstString.CopyToConstBuffer(g_sCSpSclParserZero,sizeof(stuSCL_IED_AccessPoint_Server_LDevice_LN0_DOI_SDI_DAI_Val));
	pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN0_DOI_SDI_DAI_Val->node_value = pThis->m_ConstString.CopyToConstString(sNodeValue);
	pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN0_DOI_SDI_DAI->m_Val.append(pThis->m_ConstString,pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN0_DOI_SDI_DAI_Val);

	return (stuSaxChoice*)pChoice;
}

stuSaxChoice* CSpSclParser::CB_SCL_IED_AccessPoint_Server_LDevice_LN0_DOI_SDI_SDI(void* cbParam, SSaxAttribute *pAttribute,int iAttributeCount,char* sNodeValue)
{
	S_UNUSED(pAttribute);S_UNUSED(iAttributeCount);S_UNUSED(sNodeValue);static const stuSaxChoice pChoice[] = 
	{
		{ "DAI",                            CB_SCL_IED_AccessPoint_Server_LDevice_LN0_DOI_SDI_SDI_DAI },
		{ NULL,NULL }
	};

	CSpSclParser *pThis = (CSpSclParser*)cbParam;
	pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN0_DOI_SDI_SDI = (stuSCL_IED_AccessPoint_Server_LDevice_LN0_DOI_SDI_SDI*)pThis->m_ConstString.CopyToConstBuffer(g_sCSpSclParserZero,sizeof(stuSCL_IED_AccessPoint_Server_LDevice_LN0_DOI_SDI_SDI));
	for(int i=0;i<iAttributeCount;i++,pAttribute++)
	{
		if(SString::equals(pAttribute->m_sAttrName.data(), "name"))
			pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN0_DOI_SDI_SDI->name = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
	}
	pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN0_DOI_SDI_SDI->node_value = pThis->m_ConstString.CopyToConstString(sNodeValue);
	pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN0_DOI_SDI->m_SDI.append(pThis->m_ConstString,pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN0_DOI_SDI_SDI);

	return (stuSaxChoice*)pChoice;
}

stuSaxChoice* CSpSclParser::CB_SCL_IED_AccessPoint_Server_LDevice_LN0_DOI_SDI_SDI_DAI(void* cbParam, SSaxAttribute *pAttribute,int iAttributeCount,char* sNodeValue)
{
	S_UNUSED(pAttribute);S_UNUSED(iAttributeCount);S_UNUSED(sNodeValue);static const stuSaxChoice pChoice[] = 
	{
		{ "Val",                            CB_SCL_IED_AccessPoint_Server_LDevice_LN0_DOI_SDI_SDI_DAI_Val },
		{ NULL,NULL }
	};

	CSpSclParser *pThis = (CSpSclParser*)cbParam;
	pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN0_DOI_SDI_SDI_DAI = (stuSCL_IED_AccessPoint_Server_LDevice_LN0_DOI_SDI_SDI_DAI*)pThis->m_ConstString.CopyToConstBuffer(g_sCSpSclParserZero,sizeof(stuSCL_IED_AccessPoint_Server_LDevice_LN0_DOI_SDI_SDI_DAI));
	for(int i=0;i<iAttributeCount;i++,pAttribute++)
	{
		if(SString::equals(pAttribute->m_sAttrName.data(), "name"))
			pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN0_DOI_SDI_SDI_DAI->name = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
		else if(SString::equals(pAttribute->m_sAttrName.data(), "valKind"))
			pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN0_DOI_SDI_SDI_DAI->valKind = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
	}
	pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN0_DOI_SDI_SDI_DAI->node_value = pThis->m_ConstString.CopyToConstString(sNodeValue);
	pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN0_DOI_SDI_SDI->m_DAI.append(pThis->m_ConstString,pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN0_DOI_SDI_SDI_DAI);

	return (stuSaxChoice*)pChoice;
}

stuSaxChoice* CSpSclParser::CB_SCL_IED_AccessPoint_Server_LDevice_LN0_DOI_SDI_SDI_DAI_Val(void* cbParam, SSaxAttribute *pAttribute,int iAttributeCount,char* sNodeValue)
{
	S_UNUSED(pAttribute);S_UNUSED(iAttributeCount);S_UNUSED(sNodeValue);static const stuSaxChoice pChoice[] = 
	{
		{ NULL,NULL }
	};

	CSpSclParser *pThis = (CSpSclParser*)cbParam;
	pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN0_DOI_SDI_SDI_DAI_Val = (stuSCL_IED_AccessPoint_Server_LDevice_LN0_DOI_SDI_SDI_DAI_Val*)pThis->m_ConstString.CopyToConstBuffer(g_sCSpSclParserZero,sizeof(stuSCL_IED_AccessPoint_Server_LDevice_LN0_DOI_SDI_SDI_DAI_Val));
	pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN0_DOI_SDI_SDI_DAI_Val->node_value = pThis->m_ConstString.CopyToConstString(sNodeValue);
	pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN0_DOI_SDI_SDI_DAI->m_Val.append(pThis->m_ConstString,pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN0_DOI_SDI_SDI_DAI_Val);

	return (stuSaxChoice*)pChoice;
}

stuSaxChoice* CSpSclParser::CB_SCL_IED_AccessPoint_Server_LDevice_LN0_SettingControl(void* cbParam, SSaxAttribute *pAttribute,int iAttributeCount,char* sNodeValue)
{
	S_UNUSED(pAttribute);S_UNUSED(iAttributeCount);S_UNUSED(sNodeValue);static const stuSaxChoice pChoice[] = 
	{
		{ NULL,NULL }
	};

	CSpSclParser *pThis = (CSpSclParser*)cbParam;
	pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN0_SettingControl = (stuSCL_IED_AccessPoint_Server_LDevice_LN0_SettingControl*)pThis->m_ConstString.CopyToConstBuffer(g_sCSpSclParserZero,sizeof(stuSCL_IED_AccessPoint_Server_LDevice_LN0_SettingControl));
	for(int i=0;i<iAttributeCount;i++,pAttribute++)
	{
		if(SString::equals(pAttribute->m_sAttrName.data(), "numOfSGs"))
			pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN0_SettingControl->numOfSGs = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
		else if(SString::equals(pAttribute->m_sAttrName.data(), "actSG"))
			pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN0_SettingControl->actSG = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
	}
	pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN0_SettingControl->node_value = pThis->m_ConstString.CopyToConstString(sNodeValue);
	pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN0->m_SettingControl.append(pThis->m_ConstString,pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN0_SettingControl);

	return (stuSaxChoice*)pChoice;
}

stuSaxChoice* CSpSclParser::CB_SCL_IED_AccessPoint_Server_LDevice_LN0_Inputs(void* cbParam, SSaxAttribute *pAttribute,int iAttributeCount,char* sNodeValue)
{
	S_UNUSED(pAttribute);S_UNUSED(iAttributeCount);S_UNUSED(sNodeValue);static const stuSaxChoice pChoice[] = 
	{
		{ "Private",                        CB_SCL_IED_AccessPoint_Server_LDevice_LN0_Inputs_Private },
		{ "ExtRef",                         CB_SCL_IED_AccessPoint_Server_LDevice_LN0_Inputs_ExtRef },
		{ NULL,NULL }
	};

	CSpSclParser *pThis = (CSpSclParser*)cbParam;
	pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN0_Inputs = (stuSCL_IED_AccessPoint_Server_LDevice_LN0_Inputs*)pThis->m_ConstString.CopyToConstBuffer(g_sCSpSclParserZero,sizeof(stuSCL_IED_AccessPoint_Server_LDevice_LN0_Inputs));
	pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN0_Inputs->node_value = pThis->m_ConstString.CopyToConstString(sNodeValue);
	pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN0->m_Inputs.append(pThis->m_ConstString,pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN0_Inputs);

	return (stuSaxChoice*)pChoice;
}

stuSaxChoice* CSpSclParser::CB_SCL_IED_AccessPoint_Server_LDevice_LN0_Inputs_Private(void* cbParam, SSaxAttribute *pAttribute,int iAttributeCount,char* sNodeValue)
{
	S_UNUSED(pAttribute);S_UNUSED(iAttributeCount);S_UNUSED(sNodeValue);static const stuSaxChoice pChoice[] = 
	{
		{ NULL,NULL }
	};

	CSpSclParser *pThis = (CSpSclParser*)cbParam;
	pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN0_Inputs_Private = (stuSCL_IED_AccessPoint_Server_LDevice_LN0_Inputs_Private*)pThis->m_ConstString.CopyToConstBuffer(g_sCSpSclParserZero,sizeof(stuSCL_IED_AccessPoint_Server_LDevice_LN0_Inputs_Private));
	pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN0_Inputs_Private->node_value = pThis->m_ConstString.CopyToConstString(sNodeValue);
	pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN0_Inputs->m_Private.append(pThis->m_ConstString,pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN0_Inputs_Private);

	return (stuSaxChoice*)pChoice;
}

stuSaxChoice* CSpSclParser::CB_SCL_IED_AccessPoint_Server_LDevice_LN0_Inputs_ExtRef(void* cbParam, SSaxAttribute *pAttribute,int iAttributeCount,char* sNodeValue)
{
	S_UNUSED(pAttribute);S_UNUSED(iAttributeCount);S_UNUSED(sNodeValue);static const stuSaxChoice pChoice[] = 
	{
		{ NULL,NULL }
	};

	CSpSclParser *pThis = (CSpSclParser*)cbParam;
	pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN0_Inputs_ExtRef = (stuSCL_IED_AccessPoint_Server_LDevice_LN0_Inputs_ExtRef*)pThis->m_ConstString.CopyToConstBuffer(g_sCSpSclParserZero,sizeof(stuSCL_IED_AccessPoint_Server_LDevice_LN0_Inputs_ExtRef));
	for(int i=0;i<iAttributeCount;i++,pAttribute++)
	{
		if(SString::equals(pAttribute->m_sAttrName.data(), "iedName"))
			pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN0_Inputs_ExtRef->iedName = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
		else if(SString::equals(pAttribute->m_sAttrName.data(), "prefix"))
			pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN0_Inputs_ExtRef->prefix = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
		else if(SString::equals(pAttribute->m_sAttrName.data(), "doName"))
			pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN0_Inputs_ExtRef->doName = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
		else if(SString::equals(pAttribute->m_sAttrName.data(), "lnInst"))
			pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN0_Inputs_ExtRef->lnInst = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
		else if(SString::equals(pAttribute->m_sAttrName.data(), "lnClass"))
			pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN0_Inputs_ExtRef->lnClass = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
		else if(SString::equals(pAttribute->m_sAttrName.data(), "daName"))
			pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN0_Inputs_ExtRef->daName = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
		else if(SString::equals(pAttribute->m_sAttrName.data(), "intAddr"))
			pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN0_Inputs_ExtRef->intAddr = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
		else if(SString::equals(pAttribute->m_sAttrName.data(), "ldInst"))
			pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN0_Inputs_ExtRef->ldInst = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
	}
	pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN0_Inputs_ExtRef->node_value = pThis->m_ConstString.CopyToConstString(sNodeValue);
	pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN0_Inputs->m_ExtRef.append(pThis->m_ConstString,pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN0_Inputs_ExtRef);

	return (stuSaxChoice*)pChoice;
}

stuSaxChoice* CSpSclParser::CB_SCL_IED_AccessPoint_Server_LDevice_LN0_GSEControl(void* cbParam, SSaxAttribute *pAttribute,int iAttributeCount,char* sNodeValue)
{
	S_UNUSED(pAttribute);S_UNUSED(iAttributeCount);S_UNUSED(sNodeValue);static const stuSaxChoice pChoice[] = 
	{
		{ "Private",                        CB_SCL_IED_AccessPoint_Server_LDevice_LN0_GSEControl_Private },
		{ NULL,NULL }
	};

	CSpSclParser *pThis = (CSpSclParser*)cbParam;
	pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN0_GSEControl = (stuSCL_IED_AccessPoint_Server_LDevice_LN0_GSEControl*)pThis->m_ConstString.CopyToConstBuffer(g_sCSpSclParserZero,sizeof(stuSCL_IED_AccessPoint_Server_LDevice_LN0_GSEControl));
	for(int i=0;i<iAttributeCount;i++,pAttribute++)
	{
		if(SString::equals(pAttribute->m_sAttrName.data(), "appID"))
			pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN0_GSEControl->appID = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
		else if(SString::equals(pAttribute->m_sAttrName.data(), "datSet"))
			pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN0_GSEControl->datSet = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
		else if(SString::equals(pAttribute->m_sAttrName.data(), "confRev"))
			pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN0_GSEControl->confRev = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
		else if(SString::equals(pAttribute->m_sAttrName.data(), "name"))
			pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN0_GSEControl->name = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
		else if(SString::equals(pAttribute->m_sAttrName.data(), "type"))
			pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN0_GSEControl->type = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
		else if(SString::equals(pAttribute->m_sAttrName.data(), "desc"))
			pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN0_GSEControl->desc = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
	}
	pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN0_GSEControl->node_value = pThis->m_ConstString.CopyToConstString(sNodeValue);
	pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN0->m_GSEControl.append(pThis->m_ConstString,pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN0_GSEControl);

	return (stuSaxChoice*)pChoice;
}

stuSaxChoice* CSpSclParser::CB_SCL_IED_AccessPoint_Server_LDevice_LN0_GSEControl_Private(void* cbParam, SSaxAttribute *pAttribute,int iAttributeCount,char* sNodeValue)
{
	S_UNUSED(pAttribute);S_UNUSED(iAttributeCount);S_UNUSED(sNodeValue);static const stuSaxChoice pChoice[] = 
	{
		{ "P",                              CB_SCL_IED_AccessPoint_Server_LDevice_LN0_GSEControl_Private_P },
		{ NULL,NULL }
	};

	CSpSclParser *pThis = (CSpSclParser*)cbParam;
	pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN0_GSEControl_Private = (stuSCL_IED_AccessPoint_Server_LDevice_LN0_GSEControl_Private*)pThis->m_ConstString.CopyToConstBuffer(g_sCSpSclParserZero,sizeof(stuSCL_IED_AccessPoint_Server_LDevice_LN0_GSEControl_Private));
	for(int i=0;i<iAttributeCount;i++,pAttribute++)
	{
		if(SString::equals(pAttribute->m_sAttrName.data(), "type"))
			pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN0_GSEControl_Private->type = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
	}
	pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN0_GSEControl_Private->node_value = pThis->m_ConstString.CopyToConstString(sNodeValue);
	pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN0_GSEControl->m_Private.append(pThis->m_ConstString,pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN0_GSEControl_Private);

	return (stuSaxChoice*)pChoice;
}

stuSaxChoice* CSpSclParser::CB_SCL_IED_AccessPoint_Server_LDevice_LN0_GSEControl_Private_P(void* cbParam, SSaxAttribute *pAttribute,int iAttributeCount,char* sNodeValue)
{
	S_UNUSED(pAttribute);S_UNUSED(iAttributeCount);S_UNUSED(sNodeValue);static const stuSaxChoice pChoice[] = 
	{
		{ NULL,NULL }
	};

	CSpSclParser *pThis = (CSpSclParser*)cbParam;
	pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN0_GSEControl_Private_P = (stuSCL_IED_AccessPoint_Server_LDevice_LN0_GSEControl_Private_P*)pThis->m_ConstString.CopyToConstBuffer(g_sCSpSclParserZero,sizeof(stuSCL_IED_AccessPoint_Server_LDevice_LN0_GSEControl_Private_P));
	for(int i=0;i<iAttributeCount;i++,pAttribute++)
	{
		if(SString::equals(pAttribute->m_sAttrName.data(), "sznari"))
			pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN0_GSEControl_Private_P->sznari = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
		else if(SString::equals(pAttribute->m_sAttrName.data(), "type"))
			pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN0_GSEControl_Private_P->type = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
	}
	pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN0_GSEControl_Private_P->node_value = pThis->m_ConstString.CopyToConstString(sNodeValue);
	pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN0_GSEControl_Private->m_P.append(pThis->m_ConstString,pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN0_GSEControl_Private_P);

	return (stuSaxChoice*)pChoice;
}

stuSaxChoice* CSpSclParser::CB_SCL_IED_AccessPoint_Server_LDevice_LN0_LogControl(void* cbParam, SSaxAttribute *pAttribute,int iAttributeCount,char* sNodeValue)
{
	S_UNUSED(pAttribute);S_UNUSED(iAttributeCount);S_UNUSED(sNodeValue);static const stuSaxChoice pChoice[] = 
	{
		{ "TrgOps",                         CB_SCL_IED_AccessPoint_Server_LDevice_LN0_LogControl_TrgOps },
		{ NULL,NULL }
	};

	CSpSclParser *pThis = (CSpSclParser*)cbParam;
	pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN0_LogControl = (stuSCL_IED_AccessPoint_Server_LDevice_LN0_LogControl*)pThis->m_ConstString.CopyToConstBuffer(g_sCSpSclParserZero,sizeof(stuSCL_IED_AccessPoint_Server_LDevice_LN0_LogControl));
	for(int i=0;i<iAttributeCount;i++,pAttribute++)
	{
		if(SString::equals(pAttribute->m_sAttrName.data(), "name"))
			pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN0_LogControl->name = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
		else if(SString::equals(pAttribute->m_sAttrName.data(), "desc"))
			pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN0_LogControl->desc = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
		else if(SString::equals(pAttribute->m_sAttrName.data(), "datSet"))
			pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN0_LogControl->datSet = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
		else if(SString::equals(pAttribute->m_sAttrName.data(), "intgPd"))
			pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN0_LogControl->intgPd = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
		else if(SString::equals(pAttribute->m_sAttrName.data(), "logName"))
			pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN0_LogControl->logName = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
		else if(SString::equals(pAttribute->m_sAttrName.data(), "logEna"))
			pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN0_LogControl->logEna = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
		else if(SString::equals(pAttribute->m_sAttrName.data(), "reasonCode"))
			pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN0_LogControl->reasonCode = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
	}
	pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN0_LogControl->node_value = pThis->m_ConstString.CopyToConstString(sNodeValue);
	pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN0->m_LogControl.append(pThis->m_ConstString,pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN0_LogControl);

	return (stuSaxChoice*)pChoice;
}

stuSaxChoice* CSpSclParser::CB_SCL_IED_AccessPoint_Server_LDevice_LN0_LogControl_TrgOps(void* cbParam, SSaxAttribute *pAttribute,int iAttributeCount,char* sNodeValue)
{
	S_UNUSED(pAttribute);S_UNUSED(iAttributeCount);S_UNUSED(sNodeValue);static const stuSaxChoice pChoice[] = 
	{
		{ NULL,NULL }
	};

	CSpSclParser *pThis = (CSpSclParser*)cbParam;
	pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN0_LogControl_TrgOps = (stuSCL_IED_AccessPoint_Server_LDevice_LN0_LogControl_TrgOps*)pThis->m_ConstString.CopyToConstBuffer(g_sCSpSclParserZero,sizeof(stuSCL_IED_AccessPoint_Server_LDevice_LN0_LogControl_TrgOps));
	for(int i=0;i<iAttributeCount;i++,pAttribute++)
	{
		if(SString::equals(pAttribute->m_sAttrName.data(), "qchg"))
			pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN0_LogControl_TrgOps->qchg = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
		else if(SString::equals(pAttribute->m_sAttrName.data(), "period"))
			pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN0_LogControl_TrgOps->period = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
		else if(SString::equals(pAttribute->m_sAttrName.data(), "dupd"))
			pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN0_LogControl_TrgOps->dupd = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
		else if(SString::equals(pAttribute->m_sAttrName.data(), "dchg"))
			pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN0_LogControl_TrgOps->dchg = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
	}
	pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN0_LogControl_TrgOps->node_value = pThis->m_ConstString.CopyToConstString(sNodeValue);
	pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN0_LogControl->m_TrgOps.append(pThis->m_ConstString,pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN0_LogControl_TrgOps);

	return (stuSaxChoice*)pChoice;
}

stuSaxChoice* CSpSclParser::CB_SCL_IED_AccessPoint_Server_LDevice_LN0_SampledValueControl(void* cbParam, SSaxAttribute *pAttribute,int iAttributeCount,char* sNodeValue)
{
	S_UNUSED(pAttribute);S_UNUSED(iAttributeCount);S_UNUSED(sNodeValue);static const stuSaxChoice pChoice[] = 
	{
		{ "SmvOpts",                        CB_SCL_IED_AccessPoint_Server_LDevice_LN0_SampledValueControl_SmvOpts },
		{ NULL,NULL }
	};

	CSpSclParser *pThis = (CSpSclParser*)cbParam;
	pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN0_SampledValueControl = (stuSCL_IED_AccessPoint_Server_LDevice_LN0_SampledValueControl*)pThis->m_ConstString.CopyToConstBuffer(g_sCSpSclParserZero,sizeof(stuSCL_IED_AccessPoint_Server_LDevice_LN0_SampledValueControl));
	for(int i=0;i<iAttributeCount;i++,pAttribute++)
	{
		if(SString::equals(pAttribute->m_sAttrName.data(), "name"))
			pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN0_SampledValueControl->name = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
		else if(SString::equals(pAttribute->m_sAttrName.data(), "datSet"))
			pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN0_SampledValueControl->datSet = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
		else if(SString::equals(pAttribute->m_sAttrName.data(), "confRev"))
			pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN0_SampledValueControl->confRev = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
		else if(SString::equals(pAttribute->m_sAttrName.data(), "nofASDU"))
			pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN0_SampledValueControl->nofASDU = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
		else if(SString::equals(pAttribute->m_sAttrName.data(), "smpRate"))
			pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN0_SampledValueControl->smpRate = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
		else if(SString::equals(pAttribute->m_sAttrName.data(), "smvID"))
			pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN0_SampledValueControl->smvID = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
		else if(SString::equals(pAttribute->m_sAttrName.data(), "multicast"))
			pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN0_SampledValueControl->multicast = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
		else if(SString::equals(pAttribute->m_sAttrName.data(), "desc"))
			pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN0_SampledValueControl->desc = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
	}
	pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN0_SampledValueControl->node_value = pThis->m_ConstString.CopyToConstString(sNodeValue);
	pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN0->m_SampledValueControl.append(pThis->m_ConstString,pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN0_SampledValueControl);

	return (stuSaxChoice*)pChoice;
}

stuSaxChoice* CSpSclParser::CB_SCL_IED_AccessPoint_Server_LDevice_LN0_SampledValueControl_SmvOpts(void* cbParam, SSaxAttribute *pAttribute,int iAttributeCount,char* sNodeValue)
{
	S_UNUSED(pAttribute);S_UNUSED(iAttributeCount);S_UNUSED(sNodeValue);static const stuSaxChoice pChoice[] = 
	{
		{ NULL,NULL }
	};

	CSpSclParser *pThis = (CSpSclParser*)cbParam;
	pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN0_SampledValueControl_SmvOpts = (stuSCL_IED_AccessPoint_Server_LDevice_LN0_SampledValueControl_SmvOpts*)pThis->m_ConstString.CopyToConstBuffer(g_sCSpSclParserZero,sizeof(stuSCL_IED_AccessPoint_Server_LDevice_LN0_SampledValueControl_SmvOpts));
	for(int i=0;i<iAttributeCount;i++,pAttribute++)
	{
		if(SString::equals(pAttribute->m_sAttrName.data(), "refreshTime"))
			pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN0_SampledValueControl_SmvOpts->refreshTime = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
		else if(SString::equals(pAttribute->m_sAttrName.data(), "sampleRate"))
			pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN0_SampledValueControl_SmvOpts->sampleRate = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
		else if(SString::equals(pAttribute->m_sAttrName.data(), "sampleSynchronized"))
			pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN0_SampledValueControl_SmvOpts->sampleSynchronized = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
		else if(SString::equals(pAttribute->m_sAttrName.data(), "security"))
			pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN0_SampledValueControl_SmvOpts->security = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
		else if(SString::equals(pAttribute->m_sAttrName.data(), "dataRef"))
			pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN0_SampledValueControl_SmvOpts->dataRef = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
	}
	pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN0_SampledValueControl_SmvOpts->node_value = pThis->m_ConstString.CopyToConstString(sNodeValue);
	pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN0_SampledValueControl->m_SmvOpts.append(pThis->m_ConstString,pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN0_SampledValueControl_SmvOpts);

	return (stuSaxChoice*)pChoice;
}

stuSaxChoice* CSpSclParser::CB_SCL_IED_AccessPoint_Server_LDevice_LN(void* cbParam, SSaxAttribute *pAttribute,int iAttributeCount,char* sNodeValue)
{
	S_UNUSED(pAttribute);S_UNUSED(iAttributeCount);S_UNUSED(sNodeValue);static const stuSaxChoice pChoice[] = 
	{
		{ "DOI",                            CB_SCL_IED_AccessPoint_Server_LDevice_LN_DOI },
		{ NULL,NULL }
	};

	CSpSclParser *pThis = (CSpSclParser*)cbParam;
	pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN = (stuSCL_IED_AccessPoint_Server_LDevice_LN*)pThis->m_ConstString.CopyToConstBuffer(g_sCSpSclParserZero,sizeof(stuSCL_IED_AccessPoint_Server_LDevice_LN));
	for(int i=0;i<iAttributeCount;i++,pAttribute++)
	{
		if(SString::equals(pAttribute->m_sAttrName.data(), "lnType"))
			pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN->lnType = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
		else if(SString::equals(pAttribute->m_sAttrName.data(), "lnClass"))
			pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN->lnClass = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
		else if(SString::equals(pAttribute->m_sAttrName.data(), "prefix"))
			pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN->prefix = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
		else if(SString::equals(pAttribute->m_sAttrName.data(), "inst"))
			pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN->inst = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
		else if(SString::equals(pAttribute->m_sAttrName.data(), "desc"))
			pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN->desc = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
	}
	pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN->node_value = pThis->m_ConstString.CopyToConstString(sNodeValue);
	pThis->m_p_SCL_IED_AccessPoint_Server_LDevice->m_LN.append(pThis->m_ConstString,pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN);

	return (stuSaxChoice*)pChoice;
}

stuSaxChoice* CSpSclParser::CB_SCL_IED_AccessPoint_Server_LDevice_LN_DOI(void* cbParam, SSaxAttribute *pAttribute,int iAttributeCount,char* sNodeValue)
{
	S_UNUSED(pAttribute);S_UNUSED(iAttributeCount);S_UNUSED(sNodeValue);static const stuSaxChoice pChoice[] = 
	{
		{ "SDI",                            CB_SCL_IED_AccessPoint_Server_LDevice_LN_DOI_SDI },
		{ "DAI",                            CB_SCL_IED_AccessPoint_Server_LDevice_LN_DOI_DAI },
		{ NULL,NULL }
	};

	CSpSclParser *pThis = (CSpSclParser*)cbParam;
	pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN_DOI = (stuSCL_IED_AccessPoint_Server_LDevice_LN_DOI*)pThis->m_ConstString.CopyToConstBuffer(g_sCSpSclParserZero,sizeof(stuSCL_IED_AccessPoint_Server_LDevice_LN_DOI));
	for(int i=0;i<iAttributeCount;i++,pAttribute++)
	{
		if(SString::equals(pAttribute->m_sAttrName.data(), "name"))
			pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN_DOI->name = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
		else if(SString::equals(pAttribute->m_sAttrName.data(), "desc"))
			pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN_DOI->desc = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
	}
	pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN_DOI->node_value = pThis->m_ConstString.CopyToConstString(sNodeValue);
	pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN->m_DOI.append(pThis->m_ConstString,pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN_DOI);

	return (stuSaxChoice*)pChoice;
}

stuSaxChoice* CSpSclParser::CB_SCL_IED_AccessPoint_Server_LDevice_LN_DOI_SDI(void* cbParam, SSaxAttribute *pAttribute,int iAttributeCount,char* sNodeValue)
{
	S_UNUSED(pAttribute);S_UNUSED(iAttributeCount);S_UNUSED(sNodeValue);static const stuSaxChoice pChoice[] = 
	{
		{ "SDI",                            CB_SCL_IED_AccessPoint_Server_LDevice_LN_DOI_SDI_SDI },
		{ "DAI",                            CB_SCL_IED_AccessPoint_Server_LDevice_LN_DOI_SDI_DAI },
		{ NULL,NULL }
	};

	CSpSclParser *pThis = (CSpSclParser*)cbParam;
	pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN_DOI_SDI = (stuSCL_IED_AccessPoint_Server_LDevice_LN_DOI_SDI*)pThis->m_ConstString.CopyToConstBuffer(g_sCSpSclParserZero,sizeof(stuSCL_IED_AccessPoint_Server_LDevice_LN_DOI_SDI));
	for(int i=0;i<iAttributeCount;i++,pAttribute++)
	{
		if(SString::equals(pAttribute->m_sAttrName.data(), "name"))
			pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN_DOI_SDI->name = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
		else if(SString::equals(pAttribute->m_sAttrName.data(), "desc"))
			pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN_DOI_SDI->desc = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
	}
	pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN_DOI_SDI->node_value = pThis->m_ConstString.CopyToConstString(sNodeValue);
	pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN_DOI->m_SDI.append(pThis->m_ConstString,pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN_DOI_SDI);

	return (stuSaxChoice*)pChoice;
}

stuSaxChoice* CSpSclParser::CB_SCL_IED_AccessPoint_Server_LDevice_LN_DOI_SDI_SDI(void* cbParam, SSaxAttribute *pAttribute,int iAttributeCount,char* sNodeValue)
{
	S_UNUSED(pAttribute);S_UNUSED(iAttributeCount);S_UNUSED(sNodeValue);static const stuSaxChoice pChoice[] = 
	{
		{ "SDI",                            CB_SCL_IED_AccessPoint_Server_LDevice_LN_DOI_SDI_SDI_SDI },
		{ "DAI",                            CB_SCL_IED_AccessPoint_Server_LDevice_LN_DOI_SDI_SDI_DAI },
		{ NULL,NULL }
	};

	CSpSclParser *pThis = (CSpSclParser*)cbParam;
	pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN_DOI_SDI_SDI = (stuSCL_IED_AccessPoint_Server_LDevice_LN_DOI_SDI_SDI*)pThis->m_ConstString.CopyToConstBuffer(g_sCSpSclParserZero,sizeof(stuSCL_IED_AccessPoint_Server_LDevice_LN_DOI_SDI_SDI));
	for(int i=0;i<iAttributeCount;i++,pAttribute++)
	{
		if(SString::equals(pAttribute->m_sAttrName.data(), "name"))
			pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN_DOI_SDI_SDI->name = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
	}
	pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN_DOI_SDI_SDI->node_value = pThis->m_ConstString.CopyToConstString(sNodeValue);
	pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN_DOI_SDI->m_SDI.append(pThis->m_ConstString,pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN_DOI_SDI_SDI);

	return (stuSaxChoice*)pChoice;
}

stuSaxChoice* CSpSclParser::CB_SCL_IED_AccessPoint_Server_LDevice_LN_DOI_SDI_SDI_SDI(void* cbParam, SSaxAttribute *pAttribute,int iAttributeCount,char* sNodeValue)
{
	S_UNUSED(pAttribute);S_UNUSED(iAttributeCount);S_UNUSED(sNodeValue);static const stuSaxChoice pChoice[] = 
	{
		{ "DAI",                            CB_SCL_IED_AccessPoint_Server_LDevice_LN_DOI_SDI_SDI_SDI_DAI },
		{ NULL,NULL }
	};

	CSpSclParser *pThis = (CSpSclParser*)cbParam;
	pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN_DOI_SDI_SDI_SDI = (stuSCL_IED_AccessPoint_Server_LDevice_LN_DOI_SDI_SDI_SDI*)pThis->m_ConstString.CopyToConstBuffer(g_sCSpSclParserZero,sizeof(stuSCL_IED_AccessPoint_Server_LDevice_LN_DOI_SDI_SDI_SDI));
	for(int i=0;i<iAttributeCount;i++,pAttribute++)
	{
		if(SString::equals(pAttribute->m_sAttrName.data(), "name"))
			pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN_DOI_SDI_SDI_SDI->name = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
	}
	pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN_DOI_SDI_SDI_SDI->node_value = pThis->m_ConstString.CopyToConstString(sNodeValue);
	pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN_DOI_SDI_SDI->m_SDI.append(pThis->m_ConstString,pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN_DOI_SDI_SDI_SDI);

	return (stuSaxChoice*)pChoice;
}

stuSaxChoice* CSpSclParser::CB_SCL_IED_AccessPoint_Server_LDevice_LN_DOI_SDI_SDI_SDI_DAI(void* cbParam, SSaxAttribute *pAttribute,int iAttributeCount,char* sNodeValue)
{
	S_UNUSED(pAttribute);S_UNUSED(iAttributeCount);S_UNUSED(sNodeValue);static const stuSaxChoice pChoice[] = 
	{
		{ "Val",                            CB_SCL_IED_AccessPoint_Server_LDevice_LN_DOI_SDI_SDI_SDI_DAI_Val },
		{ NULL,NULL }
	};

	CSpSclParser *pThis = (CSpSclParser*)cbParam;
	pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN_DOI_SDI_SDI_SDI_DAI = (stuSCL_IED_AccessPoint_Server_LDevice_LN_DOI_SDI_SDI_SDI_DAI*)pThis->m_ConstString.CopyToConstBuffer(g_sCSpSclParserZero,sizeof(stuSCL_IED_AccessPoint_Server_LDevice_LN_DOI_SDI_SDI_SDI_DAI));
	for(int i=0;i<iAttributeCount;i++,pAttribute++)
	{
		if(SString::equals(pAttribute->m_sAttrName.data(), "name"))
			pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN_DOI_SDI_SDI_SDI_DAI->name = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
		else if(SString::equals(pAttribute->m_sAttrName.data(), "valKind"))
			pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN_DOI_SDI_SDI_SDI_DAI->valKind = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
		else if(SString::equals(pAttribute->m_sAttrName.data(), "sAddr"))
			pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN_DOI_SDI_SDI_SDI_DAI->sAddr = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
		else if(SString::equals(pAttribute->m_sAttrName.data(), "desc"))
			pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN_DOI_SDI_SDI_SDI_DAI->desc = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
	}
	pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN_DOI_SDI_SDI_SDI_DAI->node_value = pThis->m_ConstString.CopyToConstString(sNodeValue);
	pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN_DOI_SDI_SDI_SDI->m_DAI.append(pThis->m_ConstString,pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN_DOI_SDI_SDI_SDI_DAI);

	return (stuSaxChoice*)pChoice;
}

stuSaxChoice* CSpSclParser::CB_SCL_IED_AccessPoint_Server_LDevice_LN_DOI_SDI_SDI_SDI_DAI_Val(void* cbParam, SSaxAttribute *pAttribute,int iAttributeCount,char* sNodeValue)
{
	S_UNUSED(pAttribute);S_UNUSED(iAttributeCount);S_UNUSED(sNodeValue);static const stuSaxChoice pChoice[] = 
	{
		{ NULL,NULL }
	};

	CSpSclParser *pThis = (CSpSclParser*)cbParam;
	pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN_DOI_SDI_SDI_SDI_DAI_Val = (stuSCL_IED_AccessPoint_Server_LDevice_LN_DOI_SDI_SDI_SDI_DAI_Val*)pThis->m_ConstString.CopyToConstBuffer(g_sCSpSclParserZero,sizeof(stuSCL_IED_AccessPoint_Server_LDevice_LN_DOI_SDI_SDI_SDI_DAI_Val));
	pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN_DOI_SDI_SDI_SDI_DAI_Val->node_value = pThis->m_ConstString.CopyToConstString(sNodeValue);
	pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN_DOI_SDI_SDI_SDI_DAI->m_Val.append(pThis->m_ConstString,pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN_DOI_SDI_SDI_SDI_DAI_Val);

	return (stuSaxChoice*)pChoice;
}

stuSaxChoice* CSpSclParser::CB_SCL_IED_AccessPoint_Server_LDevice_LN_DOI_SDI_SDI_DAI(void* cbParam, SSaxAttribute *pAttribute,int iAttributeCount,char* sNodeValue)
{
	S_UNUSED(pAttribute);S_UNUSED(iAttributeCount);S_UNUSED(sNodeValue);static const stuSaxChoice pChoice[] = 
	{
		{ "Val",                            CB_SCL_IED_AccessPoint_Server_LDevice_LN_DOI_SDI_SDI_DAI_Val },
		{ NULL,NULL }
	};

	CSpSclParser *pThis = (CSpSclParser*)cbParam;
	pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN_DOI_SDI_SDI_DAI = (stuSCL_IED_AccessPoint_Server_LDevice_LN_DOI_SDI_SDI_DAI*)pThis->m_ConstString.CopyToConstBuffer(g_sCSpSclParserZero,sizeof(stuSCL_IED_AccessPoint_Server_LDevice_LN_DOI_SDI_SDI_DAI));
	for(int i=0;i<iAttributeCount;i++,pAttribute++)
	{
		if(SString::equals(pAttribute->m_sAttrName.data(), "name"))
			pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN_DOI_SDI_SDI_DAI->name = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
		else if(SString::equals(pAttribute->m_sAttrName.data(), "valKind"))
			pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN_DOI_SDI_SDI_DAI->valKind = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
		else if(SString::equals(pAttribute->m_sAttrName.data(), "sAddr"))
			pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN_DOI_SDI_SDI_DAI->sAddr = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
	}
	pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN_DOI_SDI_SDI_DAI->node_value = pThis->m_ConstString.CopyToConstString(sNodeValue);
	pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN_DOI_SDI_SDI->m_DAI.append(pThis->m_ConstString,pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN_DOI_SDI_SDI_DAI);

	return (stuSaxChoice*)pChoice;
}

stuSaxChoice* CSpSclParser::CB_SCL_IED_AccessPoint_Server_LDevice_LN_DOI_SDI_SDI_DAI_Val(void* cbParam, SSaxAttribute *pAttribute,int iAttributeCount,char* sNodeValue)
{
	S_UNUSED(pAttribute);S_UNUSED(iAttributeCount);S_UNUSED(sNodeValue);static const stuSaxChoice pChoice[] = 
	{
		{ NULL,NULL }
	};

	CSpSclParser *pThis = (CSpSclParser*)cbParam;
	pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN_DOI_SDI_SDI_DAI_Val = (stuSCL_IED_AccessPoint_Server_LDevice_LN_DOI_SDI_SDI_DAI_Val*)pThis->m_ConstString.CopyToConstBuffer(g_sCSpSclParserZero,sizeof(stuSCL_IED_AccessPoint_Server_LDevice_LN_DOI_SDI_SDI_DAI_Val));
	pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN_DOI_SDI_SDI_DAI_Val->node_value = pThis->m_ConstString.CopyToConstString(sNodeValue);
	pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN_DOI_SDI_SDI_DAI->m_Val.append(pThis->m_ConstString,pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN_DOI_SDI_SDI_DAI_Val);

	return (stuSaxChoice*)pChoice;
}

stuSaxChoice* CSpSclParser::CB_SCL_IED_AccessPoint_Server_LDevice_LN_DOI_SDI_DAI(void* cbParam, SSaxAttribute *pAttribute,int iAttributeCount,char* sNodeValue)
{
	S_UNUSED(pAttribute);S_UNUSED(iAttributeCount);S_UNUSED(sNodeValue);static const stuSaxChoice pChoice[] = 
	{
		{ "Val",                            CB_SCL_IED_AccessPoint_Server_LDevice_LN_DOI_SDI_DAI_Val },
		{ NULL,NULL }
	};

	CSpSclParser *pThis = (CSpSclParser*)cbParam;
	pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN_DOI_SDI_DAI = (stuSCL_IED_AccessPoint_Server_LDevice_LN_DOI_SDI_DAI*)pThis->m_ConstString.CopyToConstBuffer(g_sCSpSclParserZero,sizeof(stuSCL_IED_AccessPoint_Server_LDevice_LN_DOI_SDI_DAI));
	for(int i=0;i<iAttributeCount;i++,pAttribute++)
	{
		if(SString::equals(pAttribute->m_sAttrName.data(), "name"))
			pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN_DOI_SDI_DAI->name = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
		else if(SString::equals(pAttribute->m_sAttrName.data(), "valKind"))
			pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN_DOI_SDI_DAI->valKind = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
		else if(SString::equals(pAttribute->m_sAttrName.data(), "sAddr"))
			pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN_DOI_SDI_DAI->sAddr = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
		else if(SString::equals(pAttribute->m_sAttrName.data(), "desc"))
			pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN_DOI_SDI_DAI->desc = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
	}
	pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN_DOI_SDI_DAI->node_value = pThis->m_ConstString.CopyToConstString(sNodeValue);
	pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN_DOI_SDI->m_DAI.append(pThis->m_ConstString,pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN_DOI_SDI_DAI);

	return (stuSaxChoice*)pChoice;
}

stuSaxChoice* CSpSclParser::CB_SCL_IED_AccessPoint_Server_LDevice_LN_DOI_SDI_DAI_Val(void* cbParam, SSaxAttribute *pAttribute,int iAttributeCount,char* sNodeValue)
{
	S_UNUSED(pAttribute);S_UNUSED(iAttributeCount);S_UNUSED(sNodeValue);static const stuSaxChoice pChoice[] = 
	{
		{ NULL,NULL }
	};

	CSpSclParser *pThis = (CSpSclParser*)cbParam;
	pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN_DOI_SDI_DAI_Val = (stuSCL_IED_AccessPoint_Server_LDevice_LN_DOI_SDI_DAI_Val*)pThis->m_ConstString.CopyToConstBuffer(g_sCSpSclParserZero,sizeof(stuSCL_IED_AccessPoint_Server_LDevice_LN_DOI_SDI_DAI_Val));
	pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN_DOI_SDI_DAI_Val->node_value = pThis->m_ConstString.CopyToConstString(sNodeValue);
	pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN_DOI_SDI_DAI->m_Val.append(pThis->m_ConstString,pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN_DOI_SDI_DAI_Val);

	return (stuSaxChoice*)pChoice;
}

stuSaxChoice* CSpSclParser::CB_SCL_IED_AccessPoint_Server_LDevice_LN_DOI_DAI(void* cbParam, SSaxAttribute *pAttribute,int iAttributeCount,char* sNodeValue)
{
	S_UNUSED(pAttribute);S_UNUSED(iAttributeCount);S_UNUSED(sNodeValue);static const stuSaxChoice pChoice[] = 
	{
		{ "Val",                            CB_SCL_IED_AccessPoint_Server_LDevice_LN_DOI_DAI_Val },
		{ NULL,NULL }
	};

	CSpSclParser *pThis = (CSpSclParser*)cbParam;
	pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN_DOI_DAI = (stuSCL_IED_AccessPoint_Server_LDevice_LN_DOI_DAI*)pThis->m_ConstString.CopyToConstBuffer(g_sCSpSclParserZero,sizeof(stuSCL_IED_AccessPoint_Server_LDevice_LN_DOI_DAI));
	for(int i=0;i<iAttributeCount;i++,pAttribute++)
	{
		if(SString::equals(pAttribute->m_sAttrName.data(), "name"))
			pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN_DOI_DAI->name = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
		else if(SString::equals(pAttribute->m_sAttrName.data(), "valKind"))
			pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN_DOI_DAI->valKind = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
		else if(SString::equals(pAttribute->m_sAttrName.data(), "sAddr"))
			pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN_DOI_DAI->sAddr = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
		else if(SString::equals(pAttribute->m_sAttrName.data(), "desc"))
			pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN_DOI_DAI->desc = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
	}
	pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN_DOI_DAI->node_value = pThis->m_ConstString.CopyToConstString(sNodeValue);
	pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN_DOI->m_DAI.append(pThis->m_ConstString,pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN_DOI_DAI);

	return (stuSaxChoice*)pChoice;
}

stuSaxChoice* CSpSclParser::CB_SCL_IED_AccessPoint_Server_LDevice_LN_DOI_DAI_Val(void* cbParam, SSaxAttribute *pAttribute,int iAttributeCount,char* sNodeValue)
{
	S_UNUSED(pAttribute);S_UNUSED(iAttributeCount);S_UNUSED(sNodeValue);static const stuSaxChoice pChoice[] = 
	{
		{ NULL,NULL }
	};

	CSpSclParser *pThis = (CSpSclParser*)cbParam;
	pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN_DOI_DAI_Val = (stuSCL_IED_AccessPoint_Server_LDevice_LN_DOI_DAI_Val*)pThis->m_ConstString.CopyToConstBuffer(g_sCSpSclParserZero,sizeof(stuSCL_IED_AccessPoint_Server_LDevice_LN_DOI_DAI_Val));
	for(int i=0;i<iAttributeCount;i++,pAttribute++)
	{
		if(SString::equals(pAttribute->m_sAttrName.data(), "sGroup"))
			pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN_DOI_DAI_Val->sGroup = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
	}
	pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN_DOI_DAI_Val->node_value = pThis->m_ConstString.CopyToConstString(sNodeValue);
	pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN_DOI_DAI->m_Val.append(pThis->m_ConstString,pThis->m_p_SCL_IED_AccessPoint_Server_LDevice_LN_DOI_DAI_Val);

	return (stuSaxChoice*)pChoice;
}

stuSaxChoice* CSpSclParser::CB_SCL_DataTypeTemplates(void* cbParam, SSaxAttribute *pAttribute,int iAttributeCount,char* sNodeValue)
{
	S_UNUSED(pAttribute);S_UNUSED(iAttributeCount);S_UNUSED(sNodeValue);static const stuSaxChoice pChoice[] = 
	{
		{ "LNodeType",                      CB_SCL_DataTypeTemplates_LNodeType },
		{ "DOType",                         CB_SCL_DataTypeTemplates_DOType },
		{ "DAType",                         CB_SCL_DataTypeTemplates_DAType },
		{ "EnumType",                       CB_SCL_DataTypeTemplates_EnumType },
		{ NULL,NULL }
	};

	CSpSclParser *pThis = (CSpSclParser*)cbParam;
	pThis->m_p_SCL_DataTypeTemplates = (stuSCL_DataTypeTemplates*)pThis->m_ConstString.CopyToConstBuffer(g_sCSpSclParserZero,sizeof(stuSCL_DataTypeTemplates));
	pThis->m_p_SCL_DataTypeTemplates->node_value = pThis->m_ConstString.CopyToConstString(sNodeValue);
	pThis->m_DataTypeTemplates.append(pThis->m_ConstString,pThis->m_p_SCL_DataTypeTemplates);

	return (stuSaxChoice*)pChoice;
}

stuSaxChoice* CSpSclParser::CB_SCL_DataTypeTemplates_LNodeType(void* cbParam, SSaxAttribute *pAttribute,int iAttributeCount,char* sNodeValue)
{
	S_UNUSED(pAttribute);S_UNUSED(iAttributeCount);S_UNUSED(sNodeValue);static const stuSaxChoice pChoice[] = 
	{
		{ "DO",                             CB_SCL_DataTypeTemplates_LNodeType_DO },
		{ NULL,NULL }
	};

	CSpSclParser *pThis = (CSpSclParser*)cbParam;
	pThis->m_p_SCL_DataTypeTemplates_LNodeType = (stuSCL_DataTypeTemplates_LNodeType*)pThis->m_ConstString.CopyToConstBuffer(g_sCSpSclParserZero,sizeof(stuSCL_DataTypeTemplates_LNodeType));
	for(int i=0;i<iAttributeCount;i++,pAttribute++)
	{
		if(SString::equals(pAttribute->m_sAttrName.data(), "lnClass"))
			pThis->m_p_SCL_DataTypeTemplates_LNodeType->lnClass = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
		else if(SString::equals(pAttribute->m_sAttrName.data(), "id"))
			pThis->m_p_SCL_DataTypeTemplates_LNodeType->id = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
		else if(SString::equals(pAttribute->m_sAttrName.data(), "desc"))
			pThis->m_p_SCL_DataTypeTemplates_LNodeType->desc = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
	}
	pThis->m_p_SCL_DataTypeTemplates_LNodeType->node_value = pThis->m_ConstString.CopyToConstString(sNodeValue);
	pThis->m_p_SCL_DataTypeTemplates->m_LNodeType.append(pThis->m_ConstString,pThis->m_p_SCL_DataTypeTemplates_LNodeType);

	return (stuSaxChoice*)pChoice;
}

stuSaxChoice* CSpSclParser::CB_SCL_DataTypeTemplates_LNodeType_DO(void* cbParam, SSaxAttribute *pAttribute,int iAttributeCount,char* sNodeValue)
{
	S_UNUSED(pAttribute);S_UNUSED(iAttributeCount);S_UNUSED(sNodeValue);static const stuSaxChoice pChoice[] = 
	{
		{ NULL,NULL }
	};

	CSpSclParser *pThis = (CSpSclParser*)cbParam;
	pThis->m_p_SCL_DataTypeTemplates_LNodeType_DO = (stuSCL_DataTypeTemplates_LNodeType_DO*)pThis->m_ConstString.CopyToConstBuffer(g_sCSpSclParserZero,sizeof(stuSCL_DataTypeTemplates_LNodeType_DO));
	for(int i=0;i<iAttributeCount;i++,pAttribute++)
	{
		if(SString::equals(pAttribute->m_sAttrName.data(), "name"))
			pThis->m_p_SCL_DataTypeTemplates_LNodeType_DO->name = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
		else if(SString::equals(pAttribute->m_sAttrName.data(), "type"))
			pThis->m_p_SCL_DataTypeTemplates_LNodeType_DO->type = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
		else if(SString::equals(pAttribute->m_sAttrName.data(), "desc"))
			pThis->m_p_SCL_DataTypeTemplates_LNodeType_DO->desc = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
		else if(SString::equals(pAttribute->m_sAttrName.data(), "transient"))
			pThis->m_p_SCL_DataTypeTemplates_LNodeType_DO->transient = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
		else if(SString::equals(pAttribute->m_sAttrName.data(), "ext"))
			pThis->m_p_SCL_DataTypeTemplates_LNodeType_DO->ext = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
		else if(SString::equals(pAttribute->m_sAttrName.data(), "eOption"))
			pThis->m_p_SCL_DataTypeTemplates_LNodeType_DO->eOption = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
	}
	pThis->m_p_SCL_DataTypeTemplates_LNodeType_DO->node_value = pThis->m_ConstString.CopyToConstString(sNodeValue);
	pThis->m_p_SCL_DataTypeTemplates_LNodeType->m_DO.append(pThis->m_ConstString,pThis->m_p_SCL_DataTypeTemplates_LNodeType_DO);

	return (stuSaxChoice*)pChoice;
}

stuSaxChoice* CSpSclParser::CB_SCL_DataTypeTemplates_DOType(void* cbParam, SSaxAttribute *pAttribute,int iAttributeCount,char* sNodeValue)
{
	S_UNUSED(pAttribute);S_UNUSED(iAttributeCount);S_UNUSED(sNodeValue);static const stuSaxChoice pChoice[] = 
	{
		{ "DA",                             CB_SCL_DataTypeTemplates_DOType_DA },
		{ "SDO",                            CB_SCL_DataTypeTemplates_DOType_SDO },
		{ NULL,NULL }
	};

	CSpSclParser *pThis = (CSpSclParser*)cbParam;
	pThis->m_p_SCL_DataTypeTemplates_DOType = (stuSCL_DataTypeTemplates_DOType*)pThis->m_ConstString.CopyToConstBuffer(g_sCSpSclParserZero,sizeof(stuSCL_DataTypeTemplates_DOType));
	for(int i=0;i<iAttributeCount;i++,pAttribute++)
	{
		if(SString::equals(pAttribute->m_sAttrName.data(), "id"))
			pThis->m_p_SCL_DataTypeTemplates_DOType->id = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
		else if(SString::equals(pAttribute->m_sAttrName.data(), "cdc"))
			pThis->m_p_SCL_DataTypeTemplates_DOType->cdc = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
		else if(SString::equals(pAttribute->m_sAttrName.data(), "desc"))
			pThis->m_p_SCL_DataTypeTemplates_DOType->desc = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
	}
	pThis->m_p_SCL_DataTypeTemplates_DOType->node_value = pThis->m_ConstString.CopyToConstString(sNodeValue);
	pThis->m_p_SCL_DataTypeTemplates->m_DOType.append(pThis->m_ConstString,pThis->m_p_SCL_DataTypeTemplates_DOType);

	return (stuSaxChoice*)pChoice;
}

stuSaxChoice* CSpSclParser::CB_SCL_DataTypeTemplates_DOType_DA(void* cbParam, SSaxAttribute *pAttribute,int iAttributeCount,char* sNodeValue)
{
	S_UNUSED(pAttribute);S_UNUSED(iAttributeCount);S_UNUSED(sNodeValue);static const stuSaxChoice pChoice[] = 
	{
		{ "Val",                            CB_SCL_DataTypeTemplates_DOType_DA_Val },
		{ NULL,NULL }
	};

	CSpSclParser *pThis = (CSpSclParser*)cbParam;
	pThis->m_p_SCL_DataTypeTemplates_DOType_DA = (stuSCL_DataTypeTemplates_DOType_DA*)pThis->m_ConstString.CopyToConstBuffer(g_sCSpSclParserZero,sizeof(stuSCL_DataTypeTemplates_DOType_DA));
	for(int i=0;i<iAttributeCount;i++,pAttribute++)
	{
		if(SString::equals(pAttribute->m_sAttrName.data(), "name"))
			pThis->m_p_SCL_DataTypeTemplates_DOType_DA->name = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
		else if(SString::equals(pAttribute->m_sAttrName.data(), "bType"))
			pThis->m_p_SCL_DataTypeTemplates_DOType_DA->bType = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
		else if(SString::equals(pAttribute->m_sAttrName.data(), "type"))
			pThis->m_p_SCL_DataTypeTemplates_DOType_DA->type = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
		else if(SString::equals(pAttribute->m_sAttrName.data(), "fc"))
			pThis->m_p_SCL_DataTypeTemplates_DOType_DA->fc = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
		else if(SString::equals(pAttribute->m_sAttrName.data(), "dchg"))
			pThis->m_p_SCL_DataTypeTemplates_DOType_DA->dchg = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
		else if(SString::equals(pAttribute->m_sAttrName.data(), "qchg"))
			pThis->m_p_SCL_DataTypeTemplates_DOType_DA->qchg = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
		else if(SString::equals(pAttribute->m_sAttrName.data(), "dupd"))
			pThis->m_p_SCL_DataTypeTemplates_DOType_DA->dupd = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
	}
	pThis->m_p_SCL_DataTypeTemplates_DOType_DA->node_value = pThis->m_ConstString.CopyToConstString(sNodeValue);
	pThis->m_p_SCL_DataTypeTemplates_DOType->m_DA.append(pThis->m_ConstString,pThis->m_p_SCL_DataTypeTemplates_DOType_DA);

	return (stuSaxChoice*)pChoice;
}

stuSaxChoice* CSpSclParser::CB_SCL_DataTypeTemplates_DOType_DA_Val(void* cbParam, SSaxAttribute *pAttribute,int iAttributeCount,char* sNodeValue)
{
	S_UNUSED(pAttribute);S_UNUSED(iAttributeCount);S_UNUSED(sNodeValue);static const stuSaxChoice pChoice[] = 
	{
		{ NULL,NULL }
	};

	CSpSclParser *pThis = (CSpSclParser*)cbParam;
	pThis->m_p_SCL_DataTypeTemplates_DOType_DA_Val = (stuSCL_DataTypeTemplates_DOType_DA_Val*)pThis->m_ConstString.CopyToConstBuffer(g_sCSpSclParserZero,sizeof(stuSCL_DataTypeTemplates_DOType_DA_Val));
	pThis->m_p_SCL_DataTypeTemplates_DOType_DA_Val->node_value = pThis->m_ConstString.CopyToConstString(sNodeValue);
	pThis->m_p_SCL_DataTypeTemplates_DOType_DA->m_Val.append(pThis->m_ConstString,pThis->m_p_SCL_DataTypeTemplates_DOType_DA_Val);

	return (stuSaxChoice*)pChoice;
}

stuSaxChoice* CSpSclParser::CB_SCL_DataTypeTemplates_DOType_SDO(void* cbParam, SSaxAttribute *pAttribute,int iAttributeCount,char* sNodeValue)
{
	S_UNUSED(pAttribute);S_UNUSED(iAttributeCount);S_UNUSED(sNodeValue);static const stuSaxChoice pChoice[] = 
	{
		{ NULL,NULL }
	};

	CSpSclParser *pThis = (CSpSclParser*)cbParam;
	pThis->m_p_SCL_DataTypeTemplates_DOType_SDO = (stuSCL_DataTypeTemplates_DOType_SDO*)pThis->m_ConstString.CopyToConstBuffer(g_sCSpSclParserZero,sizeof(stuSCL_DataTypeTemplates_DOType_SDO));
	for(int i=0;i<iAttributeCount;i++,pAttribute++)
	{
		if(SString::equals(pAttribute->m_sAttrName.data(), "name"))
			pThis->m_p_SCL_DataTypeTemplates_DOType_SDO->name = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
		else if(SString::equals(pAttribute->m_sAttrName.data(), "type"))
			pThis->m_p_SCL_DataTypeTemplates_DOType_SDO->type = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
	}
	pThis->m_p_SCL_DataTypeTemplates_DOType_SDO->node_value = pThis->m_ConstString.CopyToConstString(sNodeValue);
	pThis->m_p_SCL_DataTypeTemplates_DOType->m_SDO.append(pThis->m_ConstString,pThis->m_p_SCL_DataTypeTemplates_DOType_SDO);

	return (stuSaxChoice*)pChoice;
}

stuSaxChoice* CSpSclParser::CB_SCL_DataTypeTemplates_DAType(void* cbParam, SSaxAttribute *pAttribute,int iAttributeCount,char* sNodeValue)
{
	S_UNUSED(pAttribute);S_UNUSED(iAttributeCount);S_UNUSED(sNodeValue);static const stuSaxChoice pChoice[] = 
	{
		{ "BDA",                            CB_SCL_DataTypeTemplates_DAType_BDA },
		{ NULL,NULL }
	};

	CSpSclParser *pThis = (CSpSclParser*)cbParam;
	pThis->m_p_SCL_DataTypeTemplates_DAType = (stuSCL_DataTypeTemplates_DAType*)pThis->m_ConstString.CopyToConstBuffer(g_sCSpSclParserZero,sizeof(stuSCL_DataTypeTemplates_DAType));
	for(int i=0;i<iAttributeCount;i++,pAttribute++)
	{
		if(SString::equals(pAttribute->m_sAttrName.data(), "id"))
			pThis->m_p_SCL_DataTypeTemplates_DAType->id = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
	}
	pThis->m_p_SCL_DataTypeTemplates_DAType->node_value = pThis->m_ConstString.CopyToConstString(sNodeValue);
	pThis->m_p_SCL_DataTypeTemplates->m_DAType.append(pThis->m_ConstString,pThis->m_p_SCL_DataTypeTemplates_DAType);

	return (stuSaxChoice*)pChoice;
}

stuSaxChoice* CSpSclParser::CB_SCL_DataTypeTemplates_DAType_BDA(void* cbParam, SSaxAttribute *pAttribute,int iAttributeCount,char* sNodeValue)
{
	S_UNUSED(pAttribute);S_UNUSED(iAttributeCount);S_UNUSED(sNodeValue);static const stuSaxChoice pChoice[] = 
	{
		{ NULL,NULL }
	};

	CSpSclParser *pThis = (CSpSclParser*)cbParam;
	pThis->m_p_SCL_DataTypeTemplates_DAType_BDA = (stuSCL_DataTypeTemplates_DAType_BDA*)pThis->m_ConstString.CopyToConstBuffer(g_sCSpSclParserZero,sizeof(stuSCL_DataTypeTemplates_DAType_BDA));
	for(int i=0;i<iAttributeCount;i++,pAttribute++)
	{
		if(SString::equals(pAttribute->m_sAttrName.data(), "name"))
			pThis->m_p_SCL_DataTypeTemplates_DAType_BDA->name = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
		else if(SString::equals(pAttribute->m_sAttrName.data(), "bType"))
			pThis->m_p_SCL_DataTypeTemplates_DAType_BDA->bType = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
		else if(SString::equals(pAttribute->m_sAttrName.data(), "type"))
			pThis->m_p_SCL_DataTypeTemplates_DAType_BDA->type = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
		else if(SString::equals(pAttribute->m_sAttrName.data(), "valKind"))
			pThis->m_p_SCL_DataTypeTemplates_DAType_BDA->valKind = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
		else if(SString::equals(pAttribute->m_sAttrName.data(), "count"))
			pThis->m_p_SCL_DataTypeTemplates_DAType_BDA->count = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
	}
	pThis->m_p_SCL_DataTypeTemplates_DAType_BDA->node_value = pThis->m_ConstString.CopyToConstString(sNodeValue);
	pThis->m_p_SCL_DataTypeTemplates_DAType->m_BDA.append(pThis->m_ConstString,pThis->m_p_SCL_DataTypeTemplates_DAType_BDA);

	return (stuSaxChoice*)pChoice;
}

stuSaxChoice* CSpSclParser::CB_SCL_DataTypeTemplates_EnumType(void* cbParam, SSaxAttribute *pAttribute,int iAttributeCount,char* sNodeValue)
{
	S_UNUSED(pAttribute);S_UNUSED(iAttributeCount);S_UNUSED(sNodeValue);static const stuSaxChoice pChoice[] = 
	{
		{ "EnumVal",                        CB_SCL_DataTypeTemplates_EnumType_EnumVal },
		{ NULL,NULL }
	};

	CSpSclParser *pThis = (CSpSclParser*)cbParam;
	pThis->m_p_SCL_DataTypeTemplates_EnumType = (stuSCL_DataTypeTemplates_EnumType*)pThis->m_ConstString.CopyToConstBuffer(g_sCSpSclParserZero,sizeof(stuSCL_DataTypeTemplates_EnumType));
	for(int i=0;i<iAttributeCount;i++,pAttribute++)
	{
		if(SString::equals(pAttribute->m_sAttrName.data(), "id"))
			pThis->m_p_SCL_DataTypeTemplates_EnumType->id = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
	}
	pThis->m_p_SCL_DataTypeTemplates_EnumType->node_value = pThis->m_ConstString.CopyToConstString(sNodeValue);
	pThis->m_p_SCL_DataTypeTemplates->m_EnumType.append(pThis->m_ConstString,pThis->m_p_SCL_DataTypeTemplates_EnumType);

	return (stuSaxChoice*)pChoice;
}

stuSaxChoice* CSpSclParser::CB_SCL_DataTypeTemplates_EnumType_EnumVal(void* cbParam, SSaxAttribute *pAttribute,int iAttributeCount,char* sNodeValue)
{
	S_UNUSED(pAttribute);S_UNUSED(iAttributeCount);S_UNUSED(sNodeValue);static const stuSaxChoice pChoice[] = 
	{
		{ NULL,NULL }
	};

	CSpSclParser *pThis = (CSpSclParser*)cbParam;
	pThis->m_p_SCL_DataTypeTemplates_EnumType_EnumVal = (stuSCL_DataTypeTemplates_EnumType_EnumVal*)pThis->m_ConstString.CopyToConstBuffer(g_sCSpSclParserZero,sizeof(stuSCL_DataTypeTemplates_EnumType_EnumVal));
	for(int i=0;i<iAttributeCount;i++,pAttribute++)
	{
		if(SString::equals(pAttribute->m_sAttrName.data(), "ord"))
			pThis->m_p_SCL_DataTypeTemplates_EnumType_EnumVal->ord = pThis->m_ConstString.CopyToConstString(pAttribute->m_sAttrValue.data(),pAttribute->m_sAttrValue.length());
	}
	pThis->m_p_SCL_DataTypeTemplates_EnumType_EnumVal->node_value = pThis->m_ConstString.CopyToConstString(sNodeValue);
	pThis->m_p_SCL_DataTypeTemplates_EnumType->m_EnumVal.append(pThis->m_ConstString,pThis->m_p_SCL_DataTypeTemplates_EnumType_EnumVal);

	return (stuSaxChoice*)pChoice;
}

const char g_saxscl_string_null[1]={0};

//////////////////////////////////////////////////////////////////////////
// 描    述:  处理所有的数据类型模板
// 作    者:  邵凯田
// 创建时间:  2015-12-25 14:41
// 参数说明:  void
// 返 回 值:  void
//////////////////////////////////////////////////////////////////////////
void CSpSclParserEx::ProcessDataTypeTemplates()
{
	stuSCL_DataTypeTemplates *pTmpl = m_DataTypeTemplates[0];
	if(pTmpl == NULL)
		return;
	unsigned long pos=0;
	CLNodeType *pDTMLNodeType;
	stuSCL_DataTypeTemplates_LNodeType *pLNodeType = pTmpl->m_LNodeType.FetchFirst(pos);
	while(pLNodeType)
	{
		if((pDTMLNodeType=SearchDTMLNodeType((char*)pLNodeType->id)) == NULL)
		{
			NewDTMLNodeType(pLNodeType);
		}
		pLNodeType = pTmpl->m_LNodeType.FetchNext(pos);
	}
}

CSpSclParserEx::CLNodeType* CSpSclParserEx::NewDTMLNodeType(CSpSclParserEx::stuSCL_DataTypeTemplates_LNodeType *plnt)
{
	CLNodeType *p = new CLNodeType();
	p->pLNodeType = plnt;
	m_LNodeTypes.append(p);
	m_mapLNodeTypes[p->pLNodeType->id] = p;
	ProcessLNodeType(p);
	return p;
}

bool CSpSclParserEx::ProcessLNodeType(CSpSclParserEx::CLNodeType *pDTMLNodeType)
{
	CLNodeType_DO *pNewDO;
	CDOType *pDTMDO;
	stuSCL_DataTypeTemplates_LNodeType* pLNodeType = pDTMLNodeType->pLNodeType;
	unsigned long pos=0;
	stuSCL_DataTypeTemplates_DOType *pDOType;
	//stuSCL_DataTypeTemplates_DAType *pDAype;
	//stuSCL_DataTypeTemplates_EnumType *pEnumType;
	stuSCL_DataTypeTemplates_LNodeType_DO *pDO = pLNodeType->m_DO.FetchFirst(pos);
	while(pDO)
	{
		pDTMDO = SearchDTMDOType((char*)pDO->type);
		if(pDTMDO == NULL)
		{
			pDOType = SearchSclDOType((char*)pDO->type);
			if(pDOType == NULL)
			{
				LOGWARN("无效的DO类型(%s)",pDO->type);
				pDO = pLNodeType->m_DO.FetchNext(pos);
				continue;
			}
			pDTMDO = NewDTMDOType(pDOType);
		}
		else
			pDOType = pDTMDO->pDOType;
		pNewDO = new CLNodeType_DO();
		pNewDO->pDO = pDO;
		pNewDO->m_pDoType = pDTMDO;
		pDTMLNodeType->m_DO.append(pNewDO);		
		pDO = pLNodeType->m_DO.FetchNext(pos);
	}
	return true;
}


CSpSclParserEx::CDOType* CSpSclParserEx::NewDTMDOType(CSpSclParserEx::stuSCL_DataTypeTemplates_DOType *pDOType)
{
	std::string key = pDOType->id;
	CDOType* p = new CDOType();
	m_mapDOTypes[key] = p;
	p->pDOType = pDOType;
	m_DOTypes.append(p);
	ProcessDOType(p);
	return p;
}

bool CSpSclParserEx::ProcessDOType(CSpSclParserEx::CDOType *pDTMDOType)
{
	unsigned long pos;
	CDOType_SDO *pDTMSDO;
	CDOType_DA *pDTMDA;
	stuSCL_DataTypeTemplates_DOType *pDOType = pDTMDOType->pDOType;
	stuSCL_DataTypeTemplates_DOType_DA *pDA;
	stuSCL_DataTypeTemplates_DOType_SDO *pSDO;
	stuSCL_DataTypeTemplates_DOType_DA_Val *pDAVal;
	pDA = pDOType->m_DA.FetchFirst(pos);
	while(pDA)
	{
		pDTMDA = new CDOType_DA();
		pDTMDA->pDA = pDA;
		pDTMDOType->m_DA.append(pDTMDA);
		pDAVal = pDA->m_Val[0];
		if(pDAVal != NULL)
			pDTMDA->da_val = pDAVal->node_value;
		else
			pDTMDA->da_val = (char*)g_saxscl_string_null;
		pDTMDA->m_pDAType = NULL;
		pDTMDA->m_pEnumType = NULL;
		if(SString::equals(pDA->bType,"Enum"))
		{
			pDTMDA->m_pEnumType = SearchSclEnumType((char*)pDA->type);
		}
		else if(SString::equals(pDA->bType,"Struct"))
		{
			pDTMDA->m_pDAType = SearchDTMDAType((char*)pDA->type);
			if(pDTMDA->m_pDAType == NULL)
			{
				stuSCL_DataTypeTemplates_DAType *p = SearchSclDAType((char*)pDA->type);
				if(p == NULL)
				{
					LOGWARN("无效的DA.type:%s",pDA->type);
					pDA = pDOType->m_DA.FetchNext(pos);
					pDTMDOType->m_DA.remove(pDTMDA);
					continue;
				}
				pDTMDA->m_pDAType = NewDTMDAType(p);
			}
		}
		pDA = pDOType->m_DA.FetchNext(pos);
	}

	pSDO = pDOType->m_SDO.FetchFirst(pos);
	while(pSDO)
	{
		pDTMSDO = new CDOType_SDO;
		pDTMSDO->pSDO = pSDO;
		pDTMSDO->m_pDOType = SearchDTMDOType((char*)pSDO->type);
		if(pDTMSDO->m_pDOType == NULL)
		{
			stuSCL_DataTypeTemplates_DOType *p = SearchSclDOType((char*)pSDO->type);
			if(p == NULL)
			{
				delete pDTMSDO;
				LOGWARN("无效的SDO.type:%s",pSDO->type);
				pSDO = pDOType->m_SDO.FetchNext(pos);
				continue;
			}
			pDTMSDO->m_pDOType = NewDTMDOType(p);
		}
		pDTMDOType->m_SDO.append(pDTMSDO);
		pSDO = pDOType->m_SDO.FetchNext(pos);
	}
	return true;
}


CSpSclParserEx::CDAType* CSpSclParserEx::NewDTMDAType(CSpSclParserEx::stuSCL_DataTypeTemplates_DAType *pDAType)
{
	std::string key = pDAType->id;
	CDAType* p = new CDAType();
	p->pDAType = pDAType;
	m_mapDATypes[key] = p;
	m_DATypes.append(p);
	ProcessDAType(p);
	return p;
}

bool CSpSclParserEx::ProcessDAType(CSpSclParserEx::CDAType *pDTMDAType)
{
	stuSCL_DataTypeTemplates_DAType *pDAType = pDTMDAType->pDAType;
	stuSCL_DataTypeTemplates_DAType *pDATypeSub;
	unsigned long pos;
	CDAType_BDA *pDTMBDA;
	stuSCL_DataTypeTemplates_DAType_BDA *pBDA = pDAType->m_BDA.FetchFirst(pos);
	while(pBDA)
	{
		pDTMBDA = new CDAType_BDA();
		pDTMBDA->pDAType_BDA = pBDA;
		pDTMBDA->m_pDAType = NULL;
		pDTMBDA->m_pEnumType = NULL;
		if(SString::equals(pBDA->bType,"Struct"))
		{
			pDTMBDA->m_pDAType = SearchDTMDAType((char*)pBDA->type);
			if(pDTMBDA->m_pDAType == NULL)
			{
				pDATypeSub = SearchSclDAType((char*)pBDA->type);
				if(pDATypeSub == NULL)
				{
					delete pDTMBDA;
					LOGWARN("无效的BDA.type:%s",pBDA->type);
					pBDA = pDAType->m_BDA.FetchNext(pos);
					continue;
				}
				pDTMBDA->m_pDAType = NewDTMDAType(pDATypeSub);
			}
		}
		else if(SString::equals(pBDA->bType,"Enum"))
		{
			pDTMBDA->m_pEnumType = SearchSclEnumType((char*)pBDA->type);
			if(pDTMBDA->m_pEnumType == NULL)
			{
				delete pDTMBDA;
				LOGWARN("无效的BDA.type:%s",pBDA->type);
				pBDA = pDAType->m_BDA.FetchNext(pos);
				continue;
			}
		}
		pDTMDAType->m_DAType_BDA.append(pDTMBDA);
		pBDA = pDAType->m_BDA.FetchNext(pos);
	}
	return true;
}


//////////////////////////////////////////////////////////////////////////
// 描    述:  查找SCL模型中的LNodeType节点
// 作    者:  邵凯田
// 创建时间:  2015-12-25 14:35
// 参数说明:  @lnodetype
// 返 回 值:  stuSCL_DataTypeTemplates_LNodeType*
//////////////////////////////////////////////////////////////////////////
CSpSclParserEx::stuSCL_DataTypeTemplates_LNodeType* CSpSclParserEx::SearchSclLNodeType(char* lnodetype)
{
	stuSCL_DataTypeTemplates *pTmpl = m_DataTypeTemplates[0];
	if(pTmpl == NULL)
		return NULL;
	unsigned long pos=0;
	stuSCL_DataTypeTemplates_LNodeType *pLNodeType = pTmpl->m_LNodeType.FetchFirst(pos);
	while(pLNodeType)
	{
		if(SString::equals(pLNodeType->id,lnodetype))
			return pLNodeType;
		pLNodeType = pTmpl->m_LNodeType.FetchNext(pos);
	}
	return NULL;
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  查找DOType节点
// 作    者:  邵凯田
// 创建时间:  2015-12-25 14:36
// 参数说明:  @
// 返 回 值:  stuSCL_DataTypeTemplates_DOType*
//////////////////////////////////////////////////////////////////////////
CSpSclParserEx::stuSCL_DataTypeTemplates_DOType* CSpSclParserEx::SearchSclDOType(char* dotype)
{
	stuSCL_DataTypeTemplates *pTmpl = m_DataTypeTemplates[0];
	if(pTmpl == NULL)
		return NULL;
	unsigned long pos=0;
	stuSCL_DataTypeTemplates_DOType *p = pTmpl->m_DOType.FetchFirst(pos);
	while(p)
	{
		if(SString::equals(p->id,dotype))
			return p;
		p = pTmpl->m_DOType.FetchNext(pos);
	}
	return NULL;
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  查找DAType
// 作    者:  邵凯田
// 创建时间:  2015-12-25 14:37
// 参数说明:  
// 返 回 值:  stuSCL_DataTypeTemplates_DAType*
//////////////////////////////////////////////////////////////////////////
CSpSclParserEx::stuSCL_DataTypeTemplates_DAType* CSpSclParserEx::SearchSclDAType(char* datype)
{
	stuSCL_DataTypeTemplates *pTmpl = m_DataTypeTemplates[0];
	if(pTmpl == NULL)
		return NULL;
	unsigned long pos=0;
	stuSCL_DataTypeTemplates_DAType *p = pTmpl->m_DAType.FetchFirst(pos);
	while(p)
	{
		if(SString::equals(p->id,datype))
			return p;
		p = pTmpl->m_DAType.FetchNext(pos);
	}
	return NULL;
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  查找EnumType
// 作    者:  邵凯田
// 创建时间:  2015-12-25 14:37
// 参数说明:  
// 返 回 值:  stuSCL_DataTypeTemplates_EnumType*
//////////////////////////////////////////////////////////////////////////
CSpSclParserEx::stuSCL_DataTypeTemplates_EnumType* CSpSclParserEx::SearchSclEnumType(char* enumtype)
{
	stuSCL_DataTypeTemplates *pTmpl = m_DataTypeTemplates[0];
	if(pTmpl == NULL)
		return NULL;
	unsigned long pos=0;
	stuSCL_DataTypeTemplates_EnumType *p = pTmpl->m_EnumType.FetchFirst(pos);
	while(p)
	{
		if(SString::equals(p->id,enumtype))
			return p;
		p = pTmpl->m_EnumType.FetchNext(pos);
	}
	return NULL;
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  检查处理后的LNodeType
// 作    者:  邵凯田
// 创建时间:  2015-12-25 14:37
// 参数说明:  
// 返 回 值:  CLNodeType*
//////////////////////////////////////////////////////////////////////////
CSpSclParserEx::CLNodeType* CSpSclParserEx::SearchDTMLNodeType(char* lnodetype)
{
	std::string key = lnodetype;
	return m_mapLNodeTypes[key];
}
// 
// //////////////////////////////////////////////////////////////////////////
// // 描    述:  新建LNodeType对象
// // 作    者:  邵凯田
// // 创建时间:  2015-12-25 14:38
// // 参数说明:  
// // 返 回 值:  CLNodeType*
// //////////////////////////////////////////////////////////////////////////
// CSpSclParserEx::CLNodeType* CSpSclParserEx::NewLNodeType(CSpSclParserEx::stuSCL_DataTypeTemplates_LNodeType *plnt)
// {
// 	std::string key = plnt->id;
// 	CLNodeType* pLNodeType = m_mapLNodeTypes[key];
// 	if(pLNodeType != NULL)
// 		return pLNodeType;
// 	pLNodeType = new CLNodeType();
// 	pLNodeType->pLNodeType = plnt;
// 	m_mapLNodeTypes[key] = pLNodeType;
// 	m_LNodeTypes.append(pLNodeType);
// 	ProcessLNodeType(plnt);
// 	return pLNodeType;
// }

CSpSclParserEx::CDOType* CSpSclParserEx::SearchDTMDOType(char *id)
{
	std::string key = id;
	return m_mapDOTypes[key];
}

CSpSclParserEx::CDAType* CSpSclParserEx::SearchDTMDAType(char *id)
{
	std::string key = id;
	return m_mapDATypes[key];
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  根据IED名称查找IED指针
// 作    者:  邵凯田
// 创建时间:  2015-12-23 16:31
// 参数说明:  @ied_name
// 返 回 值:  stuSCL_IED*, NULL表示未找到
//////////////////////////////////////////////////////////////////////////
CSpSclParserEx::stuSCL_IED* CSpSclParserEx::SearchIedByName(char *ied_name)
{
	unsigned long pos = 0;
	register CSpSclParser::stuSCL_IED *pIed;
	pIed = m_IED.FetchFirst(pos);
	while(pIed)
	{
		if(SString::equals(ied_name, pIed->name))
		{
			return pIed;
		}
		pIed = m_IED.FetchNext(pos);
	}
	return NULL;
}


//////////////////////////////////////////////////////////////////////////
// 描    述:  根据LD实例名，从当前IED中查找指定的LD
// 作    者:  邵凯田
// 创建时间:  2015-12-23 17:12
// 参数说明:  @pIed，ldInst
// 返 回 值:  stuSCL_IED_AccessPoint_Server_LDevice*，NULL表示失败
//////////////////////////////////////////////////////////////////////////
CSpSclParserEx::stuSCL_IED_AccessPoint_Server_LDevice* CSpSclParserEx::SearchLDByInst(CSpSclParserEx::stuSCL_IED *pIed,char* ldInst)
{
	unsigned long pos1,pos2,pos3;
	register stuSCL_IED_AccessPoint_Server_LDevice *pLd;
	register stuSCL_IED_AccessPoint_Server* pServer;
	register stuSCL_IED_AccessPoint* pAp = pIed->m_AccessPoint.FetchFirst(pos1);
	while(pAp)
	{
		pServer = pAp->m_Server.FetchFirst(pos2);
		while(pServer)
		{
			pLd = pServer->m_LDevice.FetchFirst(pos3);
			while(pLd)
			{
				if(SString::equals(pLd->inst,ldInst))
					return pLd;
				pLd = pServer->m_LDevice.FetchNext(pos3);
			}
			pServer = pAp->m_Server.FetchNext(pos2);
		}
		pAp = pIed->m_AccessPoint.FetchNext(pos1);
	}
	return NULL;
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  提取IED对应的所有LD
// 作    者:  邵凯田
// 创建时间:  2015-12-23 16:31
// 参数说明:  
// 返 回 值:  返回LD数量
//////////////////////////////////////////////////////////////////////////
int CSpSclParserEx::RetrieveIedLd(CSpSclParserEx::stuSCL_IED* pIed,SPtrList<CSpSclParserEx::stuSCL_IED_AccessPoint_Server_LDevice> &lds)
{
	unsigned long pos1,pos2,pos3;
	//load ld
	register stuSCL_IED_AccessPoint_Server_LDevice *pLd;
	register stuSCL_IED_AccessPoint_Server* pServer;
	register stuSCL_IED_AccessPoint* pAp = pIed->m_AccessPoint.FetchFirst(pos1);
	while(pAp)
	{
		pServer = pAp->m_Server.FetchFirst(pos2);
		while(pServer)
		{
			pLd = pServer->m_LDevice.FetchFirst(pos3);
			while(pLd)
			{
				lds.append(pLd);
				pLd = pServer->m_LDevice.FetchNext(pos3);
			}
			pServer = pAp->m_Server.FetchNext(pos2);
		}
		pAp = pIed->m_AccessPoint.FetchNext(pos1);
	}
	return lds.count();
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  查找指定条件的LN
// 作    者:  邵凯田
// 创建时间:  2015-12-23 17:08
// 参数说明:  @pThisLD表示当前LD，优先从当前LD中找该LN
// 返 回 值:  stuSCL_IED_AccessPoint_Server_LDevice_LN*， NULL表示失败
//////////////////////////////////////////////////////////////////////////
CSpSclParserEx::stuSCL_IED_AccessPoint_Server_LDevice_LN* CSpSclParserEx::SearchLn(CSpSclParserEx::stuSCL_IED *pIed,
	CSpSclParserEx::stuSCL_IED_AccessPoint_Server_LDevice* &pThisLD,
	char* ldInst,char* prefix,char* lnClass,char* lnInst)
{
	if(pThisLD == NULL || !SString::equals(ldInst,pThisLD->inst))
	{
		pThisLD = SearchLDByInst(pIed,ldInst);
		if(pThisLD == NULL)
			return NULL;
	}
	unsigned long pos1;
	stuSCL_IED_AccessPoint_Server_LDevice_LN *pLN = pThisLD->m_LN.FetchFirst(pos1);
	while(pLN)
	{
		if(SString::equals(pLN->prefix,prefix) && SString::equals(pLN->inst,lnInst) && SString::equals(pLN->lnClass,lnClass))
			return pLN;
		pLN = pThisLD->m_LN.FetchNext(pos1);
	}
	return NULL;
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  根据节点路径查找指定IED的LN节点
// 作    者:  邵凯田
// 创建时间:  2015-12-24 16:08
// 参数说明:  @pIed为对应IED
//         :  @path为路径，不含IEDNAME，如PI/GOINGGIO1.SPCSO1.stVal
// 返 回 值:  stuSCL_IED_AccessPoint_Server_LDevice_LN*, NULL表示找不到
//////////////////////////////////////////////////////////////////////////
CSpSclParserEx::stuSCL_IED_AccessPoint_Server_LDevice_LN_DOI* CSpSclParserEx::SearchDoiByPath(
	CSpSclParserEx::stuSCL_IED *pIed,const char* path)
{
	int len = strlen(path)+1;
	char *pStr = new char[len<<1];
	char *pLnStr =pStr+len;
	memcpy(pStr,path,len);
	char* ldInst=NULL,*ln_name=NULL,*do_name=NULL/*,*da_name=NULL*/;
	char* s1;
	stuSCL_IED_AccessPoint_Server_LDevice_LN_DOI *pDoi;
	stuSCL_IED_AccessPoint_Server_LDevice_LN *pLN;
	//PI/GOINGGIO1.SPCSO1.stVal
	//PI为ldname
	//GOINGGIO1为prefix+lnClass+inst
	//SPCSO1为doname
	//stVal为daname

	//SVLD/SVINGGIO1.DelayTRtg1.instMag.i
	//instMag表示SDI
	//i表示DAI
	s1 = ldInst = pStr;
	while(*s1 != '/' && *s1 != '\0')
		s1++;
	if(*s1 == '\0')
	{delete[]pStr;	return NULL;}
	*s1 = '\0';
	ln_name = ++s1;
	while(*s1 != '.' && *s1 != '\0')
		s1++;
	if(*s1 == '\0')
	{delete[]pStr;	return NULL;}
	*s1 = '\0';
	do_name = ++s1;
	while(*s1 != '.' && *s1 != '\0')
		s1++;
// 	if(*s1 == '\0')
// 	{delete[]pStr;	return NULL;}
	*s1 = '\0';
// 	da_name = ++s1;
// 	int i;
// 	len = strlen(da_name);
	
	stuSCL_IED_AccessPoint_Server_LDevice *pThisLD = SearchLDByInst(pIed,ldInst);
	if(pThisLD == NULL)
	{delete[]pStr;	return NULL;}
	if(strstr(path,"SVLD_PROT/SVINGGIO1.SAVSO1")!=NULL)
		path=path;
	unsigned long pos1,pos2;
	pLN = pThisLD->m_LN.FetchFirst(pos1);
	while(pLN)
	{
		pLnStr[0] = '\0';
		strcat(pLnStr,SString::safeChar(pLN->prefix));
		strcat(pLnStr,SString::safeChar(pLN->lnClass));
		strcat(pLnStr,SString::safeChar(pLN->inst));
		if(SString::equals(pLnStr,ln_name))
		{
			pDoi = pLN->m_DOI.FetchFirst(pos2);	
			while(pDoi)
			{
				if(SString::equals(pDoi->name,do_name))
				{
					{delete[]pStr;	return pDoi;}
				}
				pDoi = pLN->m_DOI.FetchNext(pos2);
			}
			break;
		}
		pLN = pThisLD->m_LN.FetchNext(pos1);
	}
	{delete[]pStr;	return NULL;}
}


//////////////////////////////////////////////////////////////////////////
// 描    述:  在指定的LN中查找指定名称的DO
// 作    者:  邵凯田
// 创建时间:  2015-12-23 17:40
// 参数说明:  @pLN
// 返 回 值:  stuSCL_IED_AccessPoint_Server_LDevice_LN_DOI*， NULL表示找不到
//////////////////////////////////////////////////////////////////////////
CSpSclParserEx::stuSCL_IED_AccessPoint_Server_LDevice_LN_DOI* CSpSclParserEx::SearchDoiByName(CSpSclParserEx::stuSCL_IED_AccessPoint_Server_LDevice_LN *pLN,char* doName)
{
	unsigned long pos1;
	stuSCL_IED_AccessPoint_Server_LDevice_LN_DOI *pDoi = pLN->m_DOI.FetchFirst(pos1);
	while(pDoi)
	{
		if(SString::equals(doName,pDoi->name))
			return pDoi;
		pDoi = pLN->m_DOI.FetchNext(pos1);
	}
	return NULL;
}

CSpSclParserEx::stuSCL_IED_AccessPoint_Server_LDevice_LN0_DOI* CSpSclParserEx::SearchDoiByName(CSpSclParserEx::stuSCL_IED_AccessPoint_Server_LDevice_LN0 *pLN,char* doName)
{
	unsigned long pos1;
	stuSCL_IED_AccessPoint_Server_LDevice_LN0_DOI *pDoi = pLN->m_DOI.FetchFirst(pos1);
	while(pDoi)
	{
		if(SString::equals(doName,pDoi->name))
			return pDoi;
		pDoi = pLN->m_DOI.FetchNext(pos1);
	}
	return NULL;
}


//////////////////////////////////////////////////////////////////////////
// 描    述:  根据数据集中的FCDA查找对应节点的描述信息
// 作    者:  邵凯田
// 创建时间:  2015-12-25 9:53
// 参数说明:  
// 返 回 值:  true/false
//////////////////////////////////////////////////////////////////////////
bool CSpSclParserEx::GetFcdaNodeInfo(CSpSclParserEx::stuSCL_IED_AccessPoint_Server_LDevice_LN0_DataSet_FCDA *pFCDA,
	CSpSclParserEx::stuSCL_IED *pIed,CSpSclParserEx::stuSCL_IED_AccessPoint_Server_LDevice* &pThisLD,
	char* &ln_desc, char* &do_desc, char* &da_desc, char* &dime,char* &ln_type,char* &do_type, SString *pExtAttr/*=NULL*/)
{
	S_UNUSED(pExtAttr);
	stuSCL_IED_AccessPoint_Server_LDevice_LN0 *pLN0 = NULL;
	if(SString::equals(pFCDA->lnClass, "LLN0"))
	{
		if(pThisLD == NULL || !SString::equals(pFCDA->ldInst,pThisLD->inst))
		{
			pThisLD = SearchLDByInst(pIed,(char*)pFCDA->ldInst);
			if(pThisLD == NULL)
				return false;
		}
		pLN0 = pThisLD->m_LN0[0];
		if(pLN0 == NULL)
			return false;
		ln_type = (char*)pLN0->lnType;
		CLNodeType *pLNType = SearchDTMLNodeType(ln_type);
		if(pLNType == NULL)
			return false;

		ln_desc = (char*)SString::safeChar(pLN0->desc);
		char *s1;
		stuSCL_IED_AccessPoint_Server_LDevice_LN0_DOI *pDoi;
		stuSCL_IED_AccessPoint_Server_LDevice_LN0_DOI_SDI *pSDI;
		stuSCL_IED_AccessPoint_Server_LDevice_LN0_DOI_SDI_SDI *pSDI_SDI;
		//CDOType *pDTMDOType = SearchDTMDOType(pThisLN->lnType);

		if((s1= (char*)strstr((char*)pFCDA->doName,".")) == NULL)
		{
			//CDAType *pDTMDAType = SearchDTMDAType(pFCDA->daName);
			CDOType* pDTMDO = pLNType->SearchDOTypeByName(pFCDA->doName);
			if(pDTMDO != NULL && pDTMDO->pDOType != NULL)
				do_type = (char*)pDTMDO->pDOType->id;
			pDoi = SearchDoiByName(pLN0,(char*)pFCDA->doName);
			if(pDoi == NULL)
				return false;
// 			if(pFCDA->daName == NULL && pDoi->m_DAI.count() > 0)
// 				pFCDA->daName = pDoi->m_DAI[0]->name;
			do_desc = (char*)pDoi->desc;
			if(pDoi->m_DAI.count() == 0)
				da_desc = dime = (char*)g_saxscl_string_null;
			else
			{
				stuSCL_IED_AccessPoint_Server_LDevice_LN0_DOI_DAI_Val *pDu = SearchDaiDu(pDoi);
				if(pDu != NULL)
					da_desc = (char*)pDu->node_value;
				else
					da_desc = (char*)g_saxscl_string_null;
				unsigned long pos1,pos2;
				stuSCL_IED_AccessPoint_Server_LDevice_LN0_DOI_SDI *pSDI = pDoi->m_SDI.FetchFirst(pos1);
				while(pSDI)
				{
					if(SString::equals(pSDI->name,"units"))
					{
						SString SIUnit,multiplier;
						stuSCL_IED_AccessPoint_Server_LDevice_LN0_DOI_SDI_DAI *pDai = pSDI->m_DAI.FetchFirst(pos2);
						while(pDai)
						{
							stuSCL_IED_AccessPoint_Server_LDevice_LN0_DOI_SDI_DAI_Val *pVal = pDai->m_Val[0];
							if(pVal == NULL)
							{
								pDai = pSDI->m_DAI.FetchNext(pos2);
								continue;
							}
							if(SString::equals(pDai->name,"SIUnit"))
								SIUnit = pVal->node_value;
							else if(SString::equals(pDai->name,"multiplier"))
								multiplier = pVal->node_value;
							pDai = pSDI->m_DAI.FetchNext(pos2);
						}

						if(SIUnit.length() > 0 && multiplier.length() > 0)
						{
							dime = GetUnitDesc(SIUnit.data(),multiplier.data());
						}
						break;
					}
					pSDI = pDoi->m_SDI.FetchNext(pos1);
				}
			}
		}
		else
		{
			SString sDoName = pFCDA->doName;
			int cnt = SString::GetAttributeCount(sDoName,".");
			SString do1 = SString::GetIdAttribute(1,sDoName,".");
			SString do2 = SString::GetIdAttribute(2,sDoName,".");
			unsigned long pos1,pos2;
			CDOType* pDTMDO = pLNType->SearchDOTypeByName(do1.data());
			if(pDTMDO != NULL && pDTMDO->pDOType != NULL)
			{
				pDTMDO = pDTMDO->SearchDOTypeByName(do2.data());
				if(pDTMDO != NULL)
					do_type = (char*)pDTMDO->pDOType->id;
			}

			pDoi = pLN0->m_DOI.FetchFirst(pos1);
			while(pDoi)
			{
				if(SString::equals(do1.data(),pDoi->name))
					break;
				pDoi = pLN0->m_DOI.FetchNext(pos1);
			}
			if(pDoi == NULL)
				return NULL;
			if(cnt == 2)
			{
				pSDI = pDoi->m_SDI.FetchFirst(pos2);
				while(pSDI)
				{
					if(SString::equals(do2.data(),pSDI->name))
						break;
					pSDI = pDoi->m_SDI.FetchNext(pos2);
				}
				if(pSDI == NULL)
					return false;
				do_desc = (char*)(pSDI->name==NULL?g_saxscl_string_null:pSDI->name);//TODO:desc->name
				//search dU dai
// 				if(pFCDA->daName == NULL && pSDI->m_DAI.count() > 0)
// 					pFCDA->daName = pSDI->m_DAI[0]->name;
				stuSCL_IED_AccessPoint_Server_LDevice_LN0_DOI_SDI_DAI *pSDI_DAI = pSDI->m_DAI.FetchFirst(pos2);
				while(pSDI_DAI)
				{
					if(SString::equals(pSDI_DAI->name,"dU"))
					{
						stuSCL_IED_AccessPoint_Server_LDevice_LN0_DOI_SDI_DAI_Val *pVal = pSDI_DAI->m_Val[0];
						if(pVal == NULL)
							da_desc = (char*)g_saxscl_string_null;
						else
							da_desc = (char*)pVal->node_value;
					}
					pSDI_DAI = pSDI->m_DAI.FetchNext(pos2);
				}
				pSDI_SDI = pSDI->m_SDI.FetchFirst(pos1);
				while(pSDI_SDI)
				{
					if(SString::equals(pSDI_SDI->name,"units"))
					{
						SString SIUnit,multiplier;
						stuSCL_IED_AccessPoint_Server_LDevice_LN0_DOI_SDI_SDI_DAI *pDai = pSDI_SDI->m_DAI.FetchFirst(pos2);
						while(pDai)
						{

							stuSCL_IED_AccessPoint_Server_LDevice_LN0_DOI_SDI_SDI_DAI_Val *pVal = pDai->m_Val[0];
							if(pVal == NULL)
							{
								pDai = pSDI_SDI->m_DAI.FetchNext(pos2);
								continue;
							}
							if(SString::equals(pDai->name,"SIUnit"))
								SIUnit = pVal->node_value;
							else if(SString::equals(pDai->name,"multiplier"))
								multiplier = pVal->node_value;
							pDai = pSDI_SDI->m_DAI.FetchNext(pos2);
						}

						if(SIUnit.length() > 0 && multiplier.length() > 0)
						{

							dime = GetUnitDesc(SIUnit.data(),multiplier.data());
						}
						break;
					}
					pSDI_SDI = pSDI->m_SDI.FetchNext(pos1);
				}
			}
			else
			{
				LOGWARN("DO嵌套层数太多!cnt=%d",cnt);
				return false;
			}
		}
		return true;
	}
	stuSCL_IED_AccessPoint_Server_LDevice_LN* pThisLN = SearchLn(pIed,pThisLD,(char*)pFCDA->ldInst,(char*)pFCDA->prefix,(char*)pFCDA->lnClass,(char*)pFCDA->lnInst);
	if(pThisLN == NULL)
		return false;
	ln_type = (char*)pThisLN->lnType;
	CLNodeType *pLNType = SearchDTMLNodeType(ln_type);
	if(pLNType == NULL)
		return false;
	ln_desc = (char*)pThisLN->desc;
	char *s1;
	stuSCL_IED_AccessPoint_Server_LDevice_LN_DOI *pDoi;
	stuSCL_IED_AccessPoint_Server_LDevice_LN_DOI_SDI *pSDI;
	stuSCL_IED_AccessPoint_Server_LDevice_LN_DOI_SDI_SDI *pSDI_SDI;
	//CDOType *pDTMDOType = SearchDTMDOType(pThisLN->lnType);
	
	if((s1= (char*)strstr((char*)pFCDA->doName,".")) == NULL)
	{
		//CDAType *pDTMDAType = SearchDTMDAType(pFCDA->daName);
		CDOType* pDTMDO = pLNType->SearchDOTypeByName(pFCDA->doName);
		if(pDTMDO != NULL && pDTMDO->pDOType != NULL)
			do_type = (char*)pDTMDO->pDOType->id;
		pDoi = SearchDoiByName(pThisLN,(char*)pFCDA->doName);
		if(pDoi == NULL)
			return false;
// 		if(pFCDA->daName == NULL && pDoi->m_DAI.count() > 0)
// 			pFCDA->daName = pDoi->m_DAI[0]->name;
		do_desc = (char*)pDoi->desc;
		if(pDoi->m_DAI.count() == 0)
			da_desc = dime = (char*)g_saxscl_string_null;
		else
		{
			stuSCL_IED_AccessPoint_Server_LDevice_LN_DOI_DAI_Val *pDu = SearchDaiDu(pDoi);
			if(pDu != NULL)
				da_desc = (char*)pDu->node_value;
			else
				da_desc = (char*)g_saxscl_string_null;
			unsigned long pos1,pos2;
			stuSCL_IED_AccessPoint_Server_LDevice_LN_DOI_SDI *pSDI = pDoi->m_SDI.FetchFirst(pos1);
			while(pSDI)
			{
				if(SString::equals(pSDI->name,"units"))
				{
					SString SIUnit,multiplier;
					stuSCL_IED_AccessPoint_Server_LDevice_LN_DOI_SDI_DAI *pDai = pSDI->m_DAI.FetchFirst(pos2);
					while(pDai)
					{
						stuSCL_IED_AccessPoint_Server_LDevice_LN_DOI_SDI_DAI_Val *pVal = pDai->m_Val[0];
						if(pVal == NULL)
						{
							pDai = pSDI->m_DAI.FetchNext(pos2);
							continue;
						}
						if(SString::equals(pDai->name,"SIUnit"))
							SIUnit = pVal->node_value;
						else if(SString::equals(pDai->name,"multiplier"))
							multiplier = pVal->node_value;
						pDai = pSDI->m_DAI.FetchNext(pos2);
					}

					if(SIUnit.length() > 0 && multiplier.length() > 0)
					{
						dime = GetUnitDesc(SIUnit.data(),multiplier.data());
					}
					break;
				}
				pSDI = pDoi->m_SDI.FetchNext(pos1);
			}
		}
	}
	else
	{
		SString sDoName = pFCDA->doName;
		int cnt = SString::GetAttributeCount(sDoName,".");
		SString do1 = SString::GetIdAttribute(1,sDoName,".");
		SString do2 = SString::GetIdAttribute(2,sDoName,".");
		unsigned long pos1,pos2;
		CDOType* pDTMDO = pLNType->SearchDOTypeByName(do1.data());
		if(pDTMDO != NULL && pDTMDO->pDOType != NULL)
		{
			pDTMDO = pDTMDO->SearchDOTypeByName(do2.data());
			if(pDTMDO != NULL)
				do_type = (char*)pDTMDO->pDOType->id;
		}
		pDoi = pThisLN->m_DOI.FetchFirst(pos1);
		while(pDoi)
		{
			if(SString::equals(do1.data(),pDoi->name))
				break;
			pDoi = pThisLN->m_DOI.FetchNext(pos1);
		}
		if(pDoi == NULL)
			return NULL;
		if(cnt == 2)
		{
			pSDI = pDoi->m_SDI.FetchFirst(pos2);
			while(pSDI)
			{
				if(SString::equals(do2.data(),pSDI->name))
					break;
				pSDI = pDoi->m_SDI.FetchNext(pos2);
			}
			if(pSDI == NULL)
				return false;
			do_desc = (char*)(pSDI->desc==NULL?g_saxscl_string_null:pSDI->desc);
			//search dU dai
// 			if(pFCDA->daName == NULL && pSDI->m_DAI.count() > 0)
// 				pFCDA->daName = pSDI->m_DAI[0]->name;
			stuSCL_IED_AccessPoint_Server_LDevice_LN_DOI_SDI_DAI *pSDI_DAI = pSDI->m_DAI.FetchFirst(pos2);
			while(pSDI_DAI)
			{
				if(SString::equals(pSDI_DAI->name,"dU"))
				{
					stuSCL_IED_AccessPoint_Server_LDevice_LN_DOI_SDI_DAI_Val *pVal = pSDI_DAI->m_Val[0];
					if(pVal == NULL)
						da_desc = (char*)g_saxscl_string_null;
					else
						da_desc = (char*)pVal->node_value;
				}
				pSDI_DAI = pSDI->m_DAI.FetchNext(pos2);
			}
			pSDI_SDI = pSDI->m_SDI.FetchFirst(pos1);
			while(pSDI_SDI)
			{
				if(SString::equals(pSDI_SDI->name,"units"))
				{
					SString SIUnit,multiplier;
					stuSCL_IED_AccessPoint_Server_LDevice_LN_DOI_SDI_SDI_DAI *pDai = pSDI_SDI->m_DAI.FetchFirst(pos2);
					while(pDai)
					{

						stuSCL_IED_AccessPoint_Server_LDevice_LN_DOI_SDI_SDI_DAI_Val *pVal = pDai->m_Val[0];
						if(pVal == NULL)
						{
							pDai = pSDI_SDI->m_DAI.FetchNext(pos2);
							continue;
						}
						if(SString::equals(pDai->name,"SIUnit"))
							SIUnit = pVal->node_value;
						else if(SString::equals(pDai->name,"multiplier"))
							multiplier = pVal->node_value;
						pDai = pSDI_SDI->m_DAI.FetchNext(pos2);
					}

					if(SIUnit.length() > 0 && multiplier.length() > 0)
					{

						dime = GetUnitDesc(SIUnit.data(),multiplier.data());
					}
					break;
				}
				pSDI_SDI = pSDI->m_SDI.FetchNext(pos1);
			}
		}
		else
		{
			LOGWARN("DO嵌套层数太多!cnt=%d",cnt);
			return false;
		}
	}
	//do_desc = (char*)

	return true;
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  根据节点路径查找对应节点的描述信息
// 作    者:  邵凯田
// 创建时间:  2015-12-25 9:53
// 参数说明:  
// 返 回 值:  true/false
//////////////////////////////////////////////////////////////////////////
bool CSpSclParserEx::GetPathNodeInfo(char* path,CSpSclParserEx::stuSCL_IED *pIed,
	char* &ln_desc, char* &do_desc, char* &da_desc, char* &dime, SString *pExtAttr/*=NULL*/)
{
	S_UNUSED(pExtAttr);
	int len = strlen(path)+1;
	char *pStr = new char[len<<1];
	//char *pLnStr =pStr+len;
	memcpy(pStr,path,len);
	char* ldInst=NULL,*ln_name=NULL,*do_name1=NULL,*do_name2=NULL/*,*da_name=NULL*/;
	char* s1;
	stuSCL_IED_AccessPoint_Server_LDevice_LN_DOI *pDoi;
	//PI/GOINGGIO1.SPCSO1.stVal
	//PI为ldname
	//GOINGGIO1为prefix+lnClass+inst
	//SPCSO1为doname
	//stVal为daname

	//SVLD/SVINGGIO1.DelayTRtg1.instMag.i
	//instMag表示SDI
	//i表示DAI
	s1 = ldInst = pStr;
	while(*s1 != '/' && *s1 != '\0')
		s1++;
	if(*s1 == '\0')
	{delete[]pStr;	return false;}
	*s1 = '\0';
	ln_name = ++s1;
	while(*s1 != '.' && *s1 != '\0')
		s1++;
	if(*s1 == '\0')
	{delete[]pStr;	return false;}
	*s1 = '\0';
	do_name1 = ++s1;
	while(*s1 != '.' && *s1 != '\0')
		s1++;
	if(*s1 == '\0')
	{delete[]pStr;	return false;}
	*s1 = '\0';

	do_name2 = ++s1;
	while(*s1 != '.' && *s1 != '\0')
		s1++;
	if(*s1 == '\0')
	{delete[]pStr;	return false;}
	*s1 = '\0';


	stuSCL_IED_AccessPoint_Server_LDevice *pLD = SearchLDByInst(pIed,ldInst);
	if(pLD == NULL)
		{delete[]pStr;	return false;}
	unsigned long pos1,pos2;
	SString ln_fullname;
	stuSCL_IED_AccessPoint_Server_LDevice_LN* pThisLN = pLD->m_LN.FetchFirst(pos1);
	while(pThisLN)
	{
		ln_fullname = pThisLN->prefix;
		ln_fullname += pThisLN->lnClass;
		ln_fullname += pThisLN->inst;
		if(SString::equals(ln_fullname.data(),ln_name))
		{
			break;
		}
		pThisLN = pLD->m_LN.FetchNext(pos1);
	}
	if(pThisLN == NULL)
		{delete[]pStr;	return false;}
	ln_desc = (char*)pThisLN->desc;
	

	stuSCL_IED_AccessPoint_Server_LDevice_LN_DOI_SDI *pSDI;
	stuSCL_IED_AccessPoint_Server_LDevice_LN_DOI_SDI_SDI *pSDI_SDI;
	//CDOType *pDTMDOType = SearchDTMDOType(pThisLN->lnType);
	pDoi = pThisLN->m_DOI.FetchFirst(pos1);
	while(pDoi)
	{
		if(SString::equals(pDoi->name,do_name1))
			break;
		pDoi = pThisLN->m_DOI.FetchNext(pos1);
	}
	if(pDoi == NULL){delete pStr;return false;}
	
	if(pDoi->m_DAI.count() > 0)
	{
		//CDAType *pDTMDAType = SearchDTMDAType(pFCDA->daName);
		do_desc = (char*)pDoi->desc;
		if(pDoi->m_DAI.count() == 0)
			da_desc = dime = (char*)g_saxscl_string_null;
		else
		{
			stuSCL_IED_AccessPoint_Server_LDevice_LN_DOI_DAI_Val *pDu = SearchDaiDu(pDoi);
			if(pDu != NULL)
				da_desc = (char*)pDu->node_value;
			else
				da_desc = (char*)g_saxscl_string_null;
			unsigned long pos1,pos2;
			stuSCL_IED_AccessPoint_Server_LDevice_LN_DOI_SDI *pSDI = pDoi->m_SDI.FetchFirst(pos1);
			while(pSDI)
			{
				if(SString::equals(pSDI->name,"units"))
				{
					SString SIUnit,multiplier;
					stuSCL_IED_AccessPoint_Server_LDevice_LN_DOI_SDI_DAI *pDai = pSDI->m_DAI.FetchFirst(pos2);
					while(pDai)
					{
						stuSCL_IED_AccessPoint_Server_LDevice_LN_DOI_SDI_DAI_Val *pVal = pDai->m_Val[0];
						if(pVal == NULL)
						{
							pDai = pSDI->m_DAI.FetchNext(pos2);
							continue;
						}
						if(SString::equals(pDai->name,"SIUnit"))
							SIUnit = pVal->node_value;
						else if(SString::equals(pDai->name,"multiplier"))
							multiplier = pVal->node_value;
						pDai = pSDI->m_DAI.FetchNext(pos2);
					}

					if(SIUnit.length() > 0 && multiplier.length() > 0)
					{
						dime = GetUnitDesc(SIUnit.data(),multiplier.data());
					}
					break;
				}
				pSDI = pDoi->m_SDI.FetchNext(pos1);
			}
		}
	}
	else
	{
		//if(cnt == 2)
		{
			pSDI = pDoi->m_SDI.FetchFirst(pos2);
			while(pSDI)
			{
				if(SString::equals(do_name2,pSDI->name))
					break;
				pSDI = pDoi->m_SDI.FetchNext(pos2);
			}
			if(pSDI == NULL)
			{
				delete pStr;
				return false;
			}
			do_desc = (char*)(pSDI->desc==NULL?g_saxscl_string_null:pSDI->desc);
			//search dU dai
			stuSCL_IED_AccessPoint_Server_LDevice_LN_DOI_SDI_DAI *pSDI_DAI = pSDI->m_DAI.FetchFirst(pos2);
			while(pSDI_DAI)
			{
				if(SString::equals(pSDI_DAI->name,"dU"))
				{
					stuSCL_IED_AccessPoint_Server_LDevice_LN_DOI_SDI_DAI_Val *pVal = pSDI_DAI->m_Val[0];
					if(pVal == NULL)
						da_desc = (char*)g_saxscl_string_null;
					else
						da_desc = (char*)pVal->node_value;
				}
				pSDI_DAI = pSDI->m_DAI.FetchFirst(pos2);
			}
			pSDI_SDI = pSDI->m_SDI.FetchFirst(pos1);
			while(pSDI_SDI)
			{
				if(SString::equals(pSDI_SDI->name,"units"))
				{
					SString SIUnit,multiplier;
					stuSCL_IED_AccessPoint_Server_LDevice_LN_DOI_SDI_SDI_DAI *pDai = pSDI_SDI->m_DAI.FetchFirst(pos2);
					while(pDai)
					{

						stuSCL_IED_AccessPoint_Server_LDevice_LN_DOI_SDI_SDI_DAI_Val *pVal = pDai->m_Val[0];
						if(pVal == NULL)
						{
							pDai = pSDI_SDI->m_DAI.FetchNext(pos2);
							continue;
						}
						if(SString::equals(pDai->name,"SIUnit"))
							SIUnit = pVal->node_value;
						else if(SString::equals(pDai->name,"multiplier"))
							multiplier = pVal->node_value;
						pDai = pSDI_SDI->m_DAI.FetchNext(pos2);
					}

					if(SIUnit.length() > 0 && multiplier.length() > 0)
					{

						dime = GetUnitDesc(SIUnit.data(),multiplier.data());
					}
					break;
				}
				pSDI_SDI = pSDI->m_SDI.FetchNext(pos1);
			}
		}
// 		else
// 		{
// 			LOGWARN("DO嵌套层数太多!cnt=%d",cnt);
// 			delete pStr;
// 			return false;
// 		}
	}
	//do_desc = (char*)
	delete pStr;
	return true;
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  取指定DO中的DAI下的dU节点的Value子节点
// 作    者:  邵凯田
// 创建时间:  2015-12-23 19:11
// 参数说明:  @pDoi
// 返 回 值:  stuSCL_IED_AccessPoint_Server_LDevice_LN_DOI_SDI_DAI_Val*, NULL表示未找到
//////////////////////////////////////////////////////////////////////////
CSpSclParserEx::stuSCL_IED_AccessPoint_Server_LDevice_LN_DOI_DAI_Val* CSpSclParserEx::SearchDaiDu(CSpSclParserEx::stuSCL_IED_AccessPoint_Server_LDevice_LN_DOI *pDoi)
{
	unsigned long pos1;
	stuSCL_IED_AccessPoint_Server_LDevice_LN_DOI_DAI *pDai = pDoi->m_DAI.FetchFirst(pos1);
	while(pDai)
	{
		if(SString::equals(pDai->name,"dU"))
		{
			return pDai->m_Val[0];
		}
		pDai = pDoi->m_DAI.FetchNext(pos1);
	}
	return NULL;
}

CSpSclParserEx::stuSCL_IED_AccessPoint_Server_LDevice_LN0_DOI_DAI_Val* CSpSclParserEx::SearchDaiDu(CSpSclParserEx::stuSCL_IED_AccessPoint_Server_LDevice_LN0_DOI *pDoi)
{
	unsigned long pos1;
	stuSCL_IED_AccessPoint_Server_LDevice_LN0_DOI_DAI *pDai = pDoi->m_DAI.FetchFirst(pos1);
	while(pDai)
	{
		if(SString::equals(pDai->name,"dU"))
		{
			return pDai->m_Val[0];
		}
		pDai = pDoi->m_DAI.FetchNext(pos1);
	}
	return NULL;
}


//////////////////////////////////////////////////////////////////////////
// 描    述:  取指定LD对应的访问点
// 作    者:  邵凯田
// 创建时间:  2015-12-23 19:33
// 参数说明:  
// 返 回 值:  stuSCL_IED_AccessPoint* , NULL表示找不到
//////////////////////////////////////////////////////////////////////////
CSpSclParserEx::stuSCL_IED_AccessPoint* CSpSclParserEx::SearchIedAccessPointByLd(CSpSclParserEx::stuSCL_IED *pIed,CSpSclParserEx::stuSCL_IED_AccessPoint_Server_LDevice *pThisLD)
{
	unsigned long pos1,pos2;
	register stuSCL_IED_AccessPoint_Server* pServer;
	register stuSCL_IED_AccessPoint* pAp = pIed->m_AccessPoint.FetchFirst(pos1);
	while(pAp)
	{
		pServer = pAp->m_Server.FetchFirst(pos2);
		while(pServer)
		{
			if(pServer->m_LDevice.exist(pThisLD))
				return pAp;
			pServer = pAp->m_Server.FetchNext(pos2);
		}
		pAp = pIed->m_AccessPoint.FetchNext(pos1);
	}
	return NULL;
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  取通讯段的访问点
// 作    者:  邵凯田
// 创建时间:  2015-12-23 20:42
// 参数说明:  @ied_name,ap_name
// 返 回 值:  stuSCL_Communication_SubNetwork_ConnectedAP*, NULL表示找不到
//////////////////////////////////////////////////////////////////////////
CSpSclParserEx::stuSCL_Communication_SubNetwork_ConnectedAP* CSpSclParserEx::SearchConnectAP(char* ied_name,char* ap_name)
{
	unsigned long pos1,pos2;
	stuSCL_Communication *pComm = m_Communication[0];
	if(pComm == NULL)
		return NULL;
	register stuSCL_Communication_SubNetwork_ConnectedAP *pAp;
	register stuSCL_Communication_SubNetwork* pNet = pComm->m_SubNetwork.FetchFirst(pos1);
	while(pNet)
	{
		pAp = pNet->m_ConnectedAP.FetchFirst(pos2);
		while(pAp)
		{
			if(SString::equals(pAp->iedName,ied_name) && SString::equals(pAp->apName,ap_name))
				return pAp;
			pAp = pNet->m_ConnectedAP.FetchNext(pos2);
		}
		pNet = pComm->m_SubNetwork.FetchNext(pos1);
	}
	return NULL;
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  取指定连接访问点的指定LD及控制块
// 作    者:  邵凯田
// 创建时间:  2015-12-23 20:59
// 参数说明:  @pCAP
// 返 回 值:  stuSCL_Communication_SubNetwork_ConnectedAP_GSE*, NULL表示找不到
//////////////////////////////////////////////////////////////////////////
CSpSclParserEx::stuSCL_Communication_SubNetwork_ConnectedAP_GSE* CSpSclParserEx::SearchCAPGSE(CSpSclParserEx::stuSCL_Communication_SubNetwork_ConnectedAP *pCAP,char* cbName,char* ldInst)
{
	unsigned long pos;
	register stuSCL_Communication_SubNetwork_ConnectedAP_GSE *pGse = pCAP->m_GSE.FetchFirst(pos);
	while(pGse)
	{
		if(SString::equals(pGse->cbName,cbName) && SString::equals(pGse->ldInst,ldInst))
			return pGse;
		pGse = pCAP->m_GSE.FetchNext(pos);
	}
	return NULL;
}

// 描    述:  取指定连接访问点的指定LD及控制块的SMV访问参数
// 作    者:  邵凯田
// 创建时间:  2015-12-23 20:59
// 参数说明:  @pCAP
// 返 回 值:  stuSCL_Communication_SubNetwork_ConnectedAP_SMV*, NULL表示找不到
//////////////////////////////////////////////////////////////////////////
CSpSclParserEx::stuSCL_Communication_SubNetwork_ConnectedAP_SMV* CSpSclParserEx::SearchCAPSMV(CSpSclParserEx::stuSCL_Communication_SubNetwork_ConnectedAP *pCAP,char* cbName,char* ldInst)
{
	unsigned long pos;
	register stuSCL_Communication_SubNetwork_ConnectedAP_SMV *pSmv = pCAP->m_SMV.FetchFirst(pos);
	while(pSmv)
	{
		if(SString::equals(pSmv->cbName,cbName) && SString::equals(pSmv->ldInst,ldInst))
			return pSmv;
		pSmv = pCAP->m_SMV.FetchNext(pos);
	}
	return NULL;
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  取指定GSE通讯参数的内容
// 作    者:  邵凯田
// 创建时间:  2015-12-23 21:28
// 参数说明:  
// 返 回 值:  void
//////////////////////////////////////////////////////////////////////////
void CSpSclParserEx::GetCAPGSE_Param(CSpSclParserEx::stuSCL_Communication_SubNetwork_ConnectedAP_GSE *pGSE,char* &mac, char* &appid, char* &vlan_priority, char* &vlan, char* &mintime, char* &maxtime)
{
	stuSCL_Communication_SubNetwork_ConnectedAP_GSE_MinTime *pMin = pGSE->m_MinTime[0];
	stuSCL_Communication_SubNetwork_ConnectedAP_GSE_MaxTime *pMax = pGSE->m_MaxTime[0];
	if(pMin != NULL)
		mintime = (char*)pMin->node_value;
	else 
		mintime = (char*)g_saxscl_string_null;
	if(pMax != NULL)
		maxtime = (char*)pMax->node_value;
	else
		maxtime = (char*)g_saxscl_string_null;
	mac = (char*)g_saxscl_string_null;
	appid = (char*)g_saxscl_string_null;
	vlan = (char*)g_saxscl_string_null;
	vlan_priority = (char*)g_saxscl_string_null;
	stuSCL_Communication_SubNetwork_ConnectedAP_GSE_Address *pAddr = pGSE->m_Address[0];
	if(pAddr != NULL)
	{
		unsigned long pos=0;
		stuSCL_Communication_SubNetwork_ConnectedAP_GSE_Address_P *p = pAddr->m_P.FetchFirst(pos);
		while(p)
		{
			if(SString::equals(p->type,"MAC-Address"))
				mac = (char*)p->node_value;
			else if(SString::equals(p->type,"APPID"))
				appid = (char*)p->node_value;
			else if(SString::equals(p->type,"VLAN-PRIORITY"))
				vlan_priority = (char*)p->node_value;
			else if(SString::equals(p->type,"VLAN-ID"))
				vlan = (char*)p->node_value;
			p = pAddr->m_P.FetchNext(pos);
		}
	}
}

void CSpSclParserEx::GetCAPGSE_Param(char* ied_name,char* ap_name,char* cbName,char* ldInst,char* &mac, char* &appid, char* &vlan_priority, char* &vlan, char* &mintime, char* &maxtime)
{
	unsigned long pos1,pos2,pos3,pos4;
	stuSCL_Communication *pComm = m_Communication[0];
	if(pComm == NULL)
		return ;
	register stuSCL_Communication_SubNetwork_ConnectedAP *pAp;
	register stuSCL_Communication_SubNetwork* pNet = pComm->m_SubNetwork.FetchFirst(pos1);
	while(pNet)
	{
		pAp = pNet->m_ConnectedAP.FetchFirst(pos2);
		while(pAp)
		{
			if(SString::equals(pAp->iedName,ied_name) && SString::equals(pAp->apName,ap_name))
			{
				register stuSCL_Communication_SubNetwork_ConnectedAP_GSE *pGSE = pAp->m_GSE.FetchFirst(pos3);
				while(pGSE)
				{
					if(SString::equals(pGSE->cbName,cbName) && SString::equals(pGSE->ldInst,ldInst))
					{
						stuSCL_Communication_SubNetwork_ConnectedAP_GSE_MinTime *pMin = pGSE->m_MinTime[0];
						stuSCL_Communication_SubNetwork_ConnectedAP_GSE_MaxTime *pMax = pGSE->m_MaxTime[0];
						if(pMin != NULL)
							mintime = (char*)pMin->node_value;
						else 
							mintime = (char*)g_saxscl_string_null;
						if(pMax != NULL)
							maxtime = (char*)pMax->node_value;
						else
							maxtime = (char*)g_saxscl_string_null;
						mac = (char*)g_saxscl_string_null;
						appid = (char*)g_saxscl_string_null;
						vlan = (char*)g_saxscl_string_null;
						vlan_priority = (char*)g_saxscl_string_null;
						stuSCL_Communication_SubNetwork_ConnectedAP_GSE_Address *pAddr = pGSE->m_Address[0];
						if(pAddr != NULL)
						{
							stuSCL_Communication_SubNetwork_ConnectedAP_GSE_Address_P *p = pAddr->m_P.FetchFirst(pos4);
							while(p)
							{
								if(SString::equals(p->type,"MAC-Address"))
									mac = (char*)p->node_value;
								else if(SString::equals(p->type,"APPID"))
									appid = (char*)p->node_value;
								else if(SString::equals(p->type,"VLAN-PRIORITY"))
									vlan_priority = (char*)p->node_value;
								else if(SString::equals(p->type,"VLAN-ID"))
									vlan = (char*)p->node_value;
								p = pAddr->m_P.FetchNext(pos4);
							}
						}
						return;
					}
					pGSE = pAp->m_GSE.FetchNext(pos3);
				}
			}
			pAp = pNet->m_ConnectedAP.FetchNext(pos2);
		}
		pNet = pComm->m_SubNetwork.FetchNext(pos1);
	}
	return;
}


//////////////////////////////////////////////////////////////////////////
// 描    述:  取指定SMV通讯参数的内容
// 作    者:  邵凯田
// 创建时间:  2015-12-23 21:47
// 参数说明:  
// 返 回 值:  void
//////////////////////////////////////////////////////////////////////////
void CSpSclParserEx::GetCAPSMV_Param(CSpSclParserEx::stuSCL_Communication_SubNetwork_ConnectedAP_SMV *pSMV,char* &mac, char* &appid, char* &vlan_priority, char* &vlan)
{
	mac = (char*)g_saxscl_string_null;
	appid = (char*)g_saxscl_string_null;
	vlan = (char*)g_saxscl_string_null;
	vlan_priority = (char*)g_saxscl_string_null;
	stuSCL_Communication_SubNetwork_ConnectedAP_SMV_Address *pAddr = pSMV->m_Address[0];
	if(pAddr != NULL)
	{
		unsigned long pos=0;
		stuSCL_Communication_SubNetwork_ConnectedAP_SMV_Address_P *p = pAddr->m_P.FetchFirst(pos);
		while(p)
		{
			if(SString::equals(p->type,"MAC-Address"))
				mac = (char*)p->node_value;
			else if(SString::equals(p->type,"APPID"))
				appid = (char*)p->node_value;
			else if(SString::equals(p->type,"VLAN-PRIORITY"))
				vlan_priority = (char*)p->node_value;
			else if(SString::equals(p->type,"VLAN-ID"))
				vlan = (char*)p->node_value;
			p = pAddr->m_P.FetchNext(pos);
		}
	}
}

void CSpSclParserEx::GetCAPSMV_Param(char* ied_name,char* ap_name,char* cbName,char* ldInst,char* &mac, char* &appid, char* &vlan_priority, char* &vlan)
{
	unsigned long pos1,pos2,pos3,pos4;
	stuSCL_Communication *pComm = m_Communication[0];
	if(pComm == NULL)
		return;
	register stuSCL_Communication_SubNetwork_ConnectedAP *pAp;
	register stuSCL_Communication_SubNetwork* pNet = pComm->m_SubNetwork.FetchFirst(pos1);
	while(pNet)
	{
		pAp = pNet->m_ConnectedAP.FetchFirst(pos2);
		while(pAp)
		{
			if(SString::equals(pAp->iedName,ied_name) && SString::equals(pAp->apName,ap_name))
			{
				register stuSCL_Communication_SubNetwork_ConnectedAP_SMV *pSMV = pAp->m_SMV.FetchFirst(pos3);
				while(pSMV)
				{
					if(SString::equals(pSMV->cbName,cbName) && SString::equals(pSMV->ldInst,ldInst))
					{
						mac = (char*)g_saxscl_string_null;
						appid = (char*)g_saxscl_string_null;
						vlan = (char*)g_saxscl_string_null;
						vlan_priority = (char*)g_saxscl_string_null;
						stuSCL_Communication_SubNetwork_ConnectedAP_SMV_Address *pAddr = pSMV->m_Address[0];
						if(pAddr != NULL)
						{
							//unsigned long pos=0;
							stuSCL_Communication_SubNetwork_ConnectedAP_SMV_Address_P *p = pAddr->m_P.FetchFirst(pos4);
							while(p)
							{
								if(SString::equals(p->type,"MAC-Address"))
									mac = (char*)p->node_value;
								else if(SString::equals(p->type,"APPID"))
									appid = (char*)p->node_value;
								else if(SString::equals(p->type,"VLAN-PRIORITY"))
									vlan_priority = (char*)p->node_value;
								else if(SString::equals(p->type,"VLAN-ID"))
									vlan = (char*)p->node_value;
								p = pAddr->m_P.FetchNext(pos4);
							}
						}
						return;
					}
					pSMV = pAp->m_SMV.FetchNext(pos3);
				}

			}
			pAp = pNet->m_ConnectedAP.FetchNext(pos2);
		}
		pNet = pComm->m_SubNetwork.FetchNext(pos1);
	}
	return;
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  根据Inputs中的外部引用，查找对应的的虚端子连接点，并引用返回其所有的IED、LD、DATASET以及控制块
// 作    者:  邵凯田
// 创建时间:  2015-12-24 13:54
// 参数说明:  
// 返 回 值:  stuSCL_IED_AccessPoint_Server_LDevice_LN0_DataSet_FCDA*表示找不到
//////////////////////////////////////////////////////////////////////////
CSpSclParserEx::stuSCL_IED_AccessPoint_Server_LDevice_LN0_DataSet_FCDA* CSpSclParserEx::SearchFCDAByExtref(
	CSpSclParserEx::stuSCL_IED_AccessPoint_Server_LDevice_LN0_Inputs_ExtRef *pExtRef,
	CSpSclParserEx::stuSCL_IED* &pIed,CSpSclParserEx::stuSCL_IED_AccessPoint_Server_LDevice* &pLd,
	CSpSclParserEx::stuSCL_IED_AccessPoint_Server_LDevice_LN0_DataSet* &pDataSet,
	CSpSclParserEx::stuSCL_IED_AccessPoint_Server_LDevice_LN0_SampledValueControl* &pSvcb,
	CSpSclParserEx::stuSCL_IED_AccessPoint_Server_LDevice_LN0_GSEControl* &pGocb)
{
	pIed = SearchIedByName((char*)pExtRef->iedName);
	if(pIed == NULL)
		return NULL;	
	pLd = SearchLDByInst(pIed,(char*)pExtRef->ldInst);
	if(pLd == NULL)
		return NULL;
	stuSCL_IED_AccessPoint_Server_LDevice_LN0 *pLN0 = pLd->m_LN0[0];
	if(pLN0 == NULL)
		return NULL;
	unsigned long pos1=0,pos2=0;
	register stuSCL_IED_AccessPoint_Server_LDevice_LN0_DataSet_FCDA *pFCDA = NULL;
	pDataSet = pLN0->m_DataSet.FetchFirst(pos1);
	while(pDataSet)
	{
		pFCDA = pDataSet->m_FCDA.FetchFirst(pos2);
		while(pFCDA)
		{
			if(SString::equals(pFCDA->daName,pExtRef->daName) && SString::equals(pFCDA->doName,pExtRef->doName) &&
				SString::equals(pFCDA->lnClass,pExtRef->lnClass) && SString::equals(pFCDA->lnInst,pExtRef->lnInst) && 
				SString::equals(pFCDA->ldInst,pExtRef->ldInst) && SString::equals(pFCDA->prefix,pExtRef->prefix))
			{
				//find it
				break;
			}
			pFCDA = pDataSet->m_FCDA.FetchNext(pos2);
		}
		if(pFCDA != NULL)
			break;
		pDataSet = pLN0->m_DataSet.FetchNext(pos1);
	}
	if(pFCDA == NULL)
		return NULL;
	//在当前LN0下查找引用该数据集的SMV或GSE控制块
	pSvcb = pLN0->m_SampledValueControl.FetchFirst(pos1);
	while(pSvcb)
	{
		if(SString::equals(pSvcb->datSet,pDataSet->name))
			break;
		pSvcb = pLN0->m_SampledValueControl.FetchNext(pos1);
	}
	if(pSvcb == NULL)
	{
		pGocb = pLN0->m_GSEControl.FetchFirst(pos1);
		while(pGocb)
		{
			if(SString::equals(pGocb->datSet,pDataSet->name))
				break;
			pGocb = pLN0->m_GSEControl.FetchNext(pos1);
		}
	}
	return pFCDA;
}

char* CSpSclParserEx::SearchEnumVal(CSpSclParserEx::stuSCL_DataTypeTemplates_EnumType *pEnum,const char* ord )
{
	unsigned long pos1;
	stuSCL_DataTypeTemplates_EnumType_EnumVal *pVal = pEnum->m_EnumVal.FetchFirst(pos1);
	while(pVal)
	{
		if(SString::equals(ord,pVal->ord))
			return (char*)pVal->node_value;
		pVal = pEnum->m_EnumVal.FetchNext(pos1);
	}
	return (char*)g_saxscl_string_null;
}

char* CSpSclParserEx::GetUnitDesc(const char* SIUnit,const char* multiplier)
{
	if(m_pEnumType == NULL)
		m_pEnumType = SearchSclEnumType((char*)"SIUnit");
	if(m_pmultiplier == NULL)
		m_pmultiplier = SearchSclEnumType((char*)"multiplier");
	if(m_pEnumType == NULL || m_pmultiplier == NULL)
		return (char*)g_saxscl_string_null;
	SString dime = SearchEnumVal(m_pmultiplier,multiplier);
	dime +=SearchEnumVal(m_pEnumType,SIUnit);
	if(dime.length() == 0)
		return (char*)g_saxscl_string_null;
	return (char*)m_ConstString.CopyToConstString(dime.data(),dime.length());
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  取数据项的基础数据类型描述
// 作    者:  邵凯田
// 创建时间:  2016-1-6 14:43
// 参数说明:  
// 返 回 值:  char*
//////////////////////////////////////////////////////////////////////////
char* CSpSclParserEx::GetDaValType(const char* do_type,const char* do_name,const char* da_name)
{
	CDOType *pDoType = SearchDTMDOType((char*)do_type);
	if(pDoType == NULL)
	{
		if(SString::equals(do_name,(char*)"DelayTRtg"))
			return (char*)"INT32";
		return (char*)g_saxscl_string_null;
	}
	unsigned long pos;	
	if(strstr(da_name,".") == NULL)
	{
		//不分层，直接取
		CDOType_DA *pDA = pDoType->m_DA.FetchFirst(pos);
		while(pDA)
		{
			if(da_name[0] == '\0' || SString::equals(pDA->pDA->name,da_name))
				return (char*)pDA->pDA->bType;
			pDA = pDoType->m_DA.FetchNext(pos);
		}
		if(SString::equals(do_name,"DelayTRtg"))
			return (char*)"INT32";
		return (char*)g_saxscl_string_null;
	}
	else
	{
		SString sDaName = da_name;
		int cnt = SString::GetAttributeCount(sDaName,".");
		SString da1 = SString::GetIdAttribute(1,sDaName,".");
		CDAType *pDaType = NULL;
		char *bType=(char*)g_saxscl_string_null;
		CDOType_DA *pDA = pDoType->m_DA.FetchFirst(pos);
		while(pDA)
		{
			if(SString::equals(pDA->pDA->name,da1.data()))
			{
				pDaType = SearchDTMDAType((char*)pDA->pDA->type);
				break;
			}
			pDA = pDoType->m_DA.FetchNext(pos);
		}
		for(int i=2;/*i<=cnt*/pDaType != NULL;i++)
		{
			CDAType_BDA *pBDA = pDaType->m_DAType_BDA.FetchFirst(pos);
			while(pBDA)
			{
				if(i>cnt||SString::equals(pBDA->pDAType_BDA->name,SString::GetIdAttribute(i,sDaName,".").data()))
				{
					bType = (char*)pBDA->pDAType_BDA->bType;
					pDaType = pBDA->m_pDAType;
					break;
				}
				pBDA = pDaType->m_DAType_BDA.FetchNext(pos);
			}
		}
		return bType;
	}
}

