/**
*
* 文 件 名 : sp_sclparser.h
* 创建日期 : 2015-12-22 08:59
* 作    者 : SspAssist(skt001@163.com)
* 修改日期 : $Date: 2015-12-22 08:59$
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

#ifndef __SAXEX_CSP_SCLPARSER_H__
#define __SAXEX_CSP_SCLPARSER_H__

#include "SSaxXml.h"
#include "SList.h"
#include <map>
#include "ssp_base_inc.h"

class SSP_BASE_EXPORT CSpSclParser : public SSaxXmlEx
{
public:
	//节点结构定义
	struct stuSCL_Private;
	struct stuSCL_Header;
	struct stuSCL_Header_History;
	struct stuSCL_Header_History_Hitem;
	struct stuSCL_Substation;
	struct stuSCL_Substation_VoltageLevel;
	struct stuSCL_Substation_VoltageLevel_Bay;
	struct stuSCL_Communication;
	struct stuSCL_Communication_SubNetwork;
	struct stuSCL_Communication_SubNetwork_BitRate;
	struct stuSCL_Communication_SubNetwork_ConnectedAP;
	struct stuSCL_Communication_SubNetwork_ConnectedAP_Private;
	struct stuSCL_Communication_SubNetwork_ConnectedAP_Address;
	struct stuSCL_Communication_SubNetwork_ConnectedAP_Address_P;
	struct stuSCL_Communication_SubNetwork_ConnectedAP_GSE;
	struct stuSCL_Communication_SubNetwork_ConnectedAP_GSE_Address;
	struct stuSCL_Communication_SubNetwork_ConnectedAP_GSE_Address_P;
	struct stuSCL_Communication_SubNetwork_ConnectedAP_GSE_MinTime;
	struct stuSCL_Communication_SubNetwork_ConnectedAP_GSE_MaxTime;
	struct stuSCL_Communication_SubNetwork_ConnectedAP_SMV;
	struct stuSCL_Communication_SubNetwork_ConnectedAP_SMV_Address;
	struct stuSCL_Communication_SubNetwork_ConnectedAP_SMV_Address_P;
	struct stuSCL_IED;
	struct stuSCL_IED_Private;
	struct stuSCL_IED_Services;
	struct stuSCL_IED_Services_DynAssociation;
	struct stuSCL_IED_Services_SettingGroups;
	struct stuSCL_IED_Services_SettingGroups_SGEdit;
	struct stuSCL_IED_Services_SettingGroups_ConfSG;
	struct stuSCL_IED_Services_GetDirectory;
	struct stuSCL_IED_Services_GetDataObjectDefinition;
	struct stuSCL_IED_Services_DataObjectDirectory;
	struct stuSCL_IED_Services_GetDataSetValue;
	struct stuSCL_IED_Services_DataSetDirectory;
	struct stuSCL_IED_Services_ConfDataSet;
	struct stuSCL_IED_Services_ReadWrite;
	struct stuSCL_IED_Services_ConfReportControl;
	struct stuSCL_IED_Services_GetCBValues;
	struct stuSCL_IED_Services_ReportSettings;
	struct stuSCL_IED_Services_GSESettings;
	struct stuSCL_IED_Services_GOOSE;
	struct stuSCL_IED_Services_FileHandling;
	struct stuSCL_IED_Services_ConfLNs;
	struct stuSCL_IED_Services_SMVSettings;
	struct stuSCL_IED_Services_SMVSettings_SmpRate;
	struct stuSCL_IED_Services_SetDataSetValue;
	struct stuSCL_IED_Services_ConfLogControl;
	struct stuSCL_IED_Services_LogSettings;
	struct stuSCL_IED_Services_TimerActivatedControl;
	struct stuSCL_IED_AccessPoint;
	struct stuSCL_IED_AccessPoint_Server;
	struct stuSCL_IED_AccessPoint_Server_Authentication;
	struct stuSCL_IED_AccessPoint_Server_LDevice;
	struct stuSCL_IED_AccessPoint_Server_LDevice_LN0;
	struct stuSCL_IED_AccessPoint_Server_LDevice_LN0_DataSet;
	struct stuSCL_IED_AccessPoint_Server_LDevice_LN0_DataSet_FCDA;
	struct stuSCL_IED_AccessPoint_Server_LDevice_LN0_ReportControl;
	struct stuSCL_IED_AccessPoint_Server_LDevice_LN0_ReportControl_TrgOps;
	struct stuSCL_IED_AccessPoint_Server_LDevice_LN0_ReportControl_OptFields;
	struct stuSCL_IED_AccessPoint_Server_LDevice_LN0_ReportControl_RptEnabled;
	struct stuSCL_IED_AccessPoint_Server_LDevice_LN0_ReportControl_Private;
	struct stuSCL_IED_AccessPoint_Server_LDevice_LN0_DOI;
	struct stuSCL_IED_AccessPoint_Server_LDevice_LN0_DOI_DAI;
	struct stuSCL_IED_AccessPoint_Server_LDevice_LN0_DOI_DAI_Val;
	struct stuSCL_IED_AccessPoint_Server_LDevice_LN0_DOI_SDI;
	struct stuSCL_IED_AccessPoint_Server_LDevice_LN0_DOI_SDI_DAI;
	struct stuSCL_IED_AccessPoint_Server_LDevice_LN0_DOI_SDI_DAI_Val;
	struct stuSCL_IED_AccessPoint_Server_LDevice_LN0_DOI_SDI_SDI;
	struct stuSCL_IED_AccessPoint_Server_LDevice_LN0_DOI_SDI_SDI_DAI;
	struct stuSCL_IED_AccessPoint_Server_LDevice_LN0_DOI_SDI_SDI_DAI_Val;
	struct stuSCL_IED_AccessPoint_Server_LDevice_LN0_SettingControl;
	struct stuSCL_IED_AccessPoint_Server_LDevice_LN0_Inputs;
	struct stuSCL_IED_AccessPoint_Server_LDevice_LN0_Inputs_Private;
	struct stuSCL_IED_AccessPoint_Server_LDevice_LN0_Inputs_ExtRef;
	struct stuSCL_IED_AccessPoint_Server_LDevice_LN0_GSEControl;
	struct stuSCL_IED_AccessPoint_Server_LDevice_LN0_GSEControl_Private;
	struct stuSCL_IED_AccessPoint_Server_LDevice_LN0_GSEControl_Private_P;
	struct stuSCL_IED_AccessPoint_Server_LDevice_LN0_LogControl;
	struct stuSCL_IED_AccessPoint_Server_LDevice_LN0_LogControl_TrgOps;
	struct stuSCL_IED_AccessPoint_Server_LDevice_LN0_SampledValueControl;
	struct stuSCL_IED_AccessPoint_Server_LDevice_LN0_SampledValueControl_SmvOpts;
	struct stuSCL_IED_AccessPoint_Server_LDevice_LN;
	struct stuSCL_IED_AccessPoint_Server_LDevice_LN_DOI;
	struct stuSCL_IED_AccessPoint_Server_LDevice_LN_DOI_SDI;
	struct stuSCL_IED_AccessPoint_Server_LDevice_LN_DOI_SDI_SDI;
	struct stuSCL_IED_AccessPoint_Server_LDevice_LN_DOI_SDI_SDI_SDI;
	struct stuSCL_IED_AccessPoint_Server_LDevice_LN_DOI_SDI_SDI_SDI_DAI;
	struct stuSCL_IED_AccessPoint_Server_LDevice_LN_DOI_SDI_SDI_SDI_DAI_Val;
	struct stuSCL_IED_AccessPoint_Server_LDevice_LN_DOI_SDI_SDI_DAI;
	struct stuSCL_IED_AccessPoint_Server_LDevice_LN_DOI_SDI_SDI_DAI_Val;
	struct stuSCL_IED_AccessPoint_Server_LDevice_LN_DOI_SDI_DAI;
	struct stuSCL_IED_AccessPoint_Server_LDevice_LN_DOI_SDI_DAI_Val;
	struct stuSCL_IED_AccessPoint_Server_LDevice_LN_DOI_DAI;
	struct stuSCL_IED_AccessPoint_Server_LDevice_LN_DOI_DAI_Val;
	struct stuSCL_DataTypeTemplates;
	struct stuSCL_DataTypeTemplates_LNodeType;
	struct stuSCL_DataTypeTemplates_LNodeType_DO;
	struct stuSCL_DataTypeTemplates_DOType;
	struct stuSCL_DataTypeTemplates_DOType_DA;
	struct stuSCL_DataTypeTemplates_DOType_DA_Val;
	struct stuSCL_DataTypeTemplates_DOType_SDO;
	struct stuSCL_DataTypeTemplates_DAType;
	struct stuSCL_DataTypeTemplates_DAType_BDA;
	struct stuSCL_DataTypeTemplates_EnumType;
	struct stuSCL_DataTypeTemplates_EnumType_EnumVal;
	struct SSP_BASE_EXPORT stuSCL_Private
	{
		const char* type;
		const char* node_value;
	};
	struct SSP_BASE_EXPORT stuSCL_Header
	{
		const char* id;
		const char* version;
		const char* revision;
		const char* toolID;
		const char* nameStructure;
		SConstPtrList<stuSCL_Header_History> m_History;
		const char* node_value;
	};
	struct SSP_BASE_EXPORT stuSCL_Header_History
	{
		SConstPtrList<stuSCL_Header_History_Hitem> m_Hitem;
		const char* node_value;
	};
	struct SSP_BASE_EXPORT stuSCL_Header_History_Hitem
	{
		const char* version;
		const char* revision;
		const char* when;
		const char* who;
		const char* what;
		const char* why;
		const char* node_value;
	};
	struct SSP_BASE_EXPORT stuSCL_Substation
	{
		const char* desc;
		const char* name;
		SConstPtrList<stuSCL_Substation_VoltageLevel> m_VoltageLevel;
		const char* node_value;
	};
	struct SSP_BASE_EXPORT stuSCL_Substation_VoltageLevel
	{
		const char* desc;
		const char* name;
		SConstPtrList<stuSCL_Substation_VoltageLevel_Bay> m_Bay;
		const char* node_value;
	};
	struct SSP_BASE_EXPORT stuSCL_Substation_VoltageLevel_Bay
	{
		const char* name;
		const char* node_value;
	};
	struct SSP_BASE_EXPORT stuSCL_Communication
	{
		const char* desc;
		SConstPtrList<stuSCL_Communication_SubNetwork> m_SubNetwork;
		const char* node_value;
	};
	struct SSP_BASE_EXPORT stuSCL_Communication_SubNetwork
	{
		const char* name;
		const char* desc;
		const char* type;
		SConstPtrList<stuSCL_Communication_SubNetwork_BitRate> m_BitRate;
		SConstPtrList<stuSCL_Communication_SubNetwork_ConnectedAP> m_ConnectedAP;
		const char* node_value;
	};
	struct SSP_BASE_EXPORT stuSCL_Communication_SubNetwork_BitRate
	{
		const char* unit;
		const char* multiplier;
		const char* node_value;
	};
	struct SSP_BASE_EXPORT stuSCL_Communication_SubNetwork_ConnectedAP
	{
		const char* iedName;
		const char* apName;
		const char* desc;
		SConstPtrList<stuSCL_Communication_SubNetwork_ConnectedAP_Private> m_Private;
		SConstPtrList<stuSCL_Communication_SubNetwork_ConnectedAP_Address> m_Address;
		SConstPtrList<stuSCL_Communication_SubNetwork_ConnectedAP_GSE> m_GSE;
		SConstPtrList<stuSCL_Communication_SubNetwork_ConnectedAP_SMV> m_SMV;
		const char* node_value;
	};
	struct SSP_BASE_EXPORT stuSCL_Communication_SubNetwork_ConnectedAP_Private
	{
		const char* type;
		const char* node_value;
	};
	struct SSP_BASE_EXPORT stuSCL_Communication_SubNetwork_ConnectedAP_Address
	{
		SConstPtrList<stuSCL_Communication_SubNetwork_ConnectedAP_Address_P> m_P;
		const char* node_value;
	};
	struct SSP_BASE_EXPORT stuSCL_Communication_SubNetwork_ConnectedAP_Address_P
	{
		const char* type;
		const char* node_value;
	};
	struct SSP_BASE_EXPORT stuSCL_Communication_SubNetwork_ConnectedAP_GSE
	{
		const char* cbName;
		const char* ldInst;
		SConstPtrList<stuSCL_Communication_SubNetwork_ConnectedAP_GSE_Address> m_Address;
		SConstPtrList<stuSCL_Communication_SubNetwork_ConnectedAP_GSE_MinTime> m_MinTime;
		SConstPtrList<stuSCL_Communication_SubNetwork_ConnectedAP_GSE_MaxTime> m_MaxTime;
		const char* node_value;
	};
	struct SSP_BASE_EXPORT stuSCL_Communication_SubNetwork_ConnectedAP_GSE_Address
	{
		SConstPtrList<stuSCL_Communication_SubNetwork_ConnectedAP_GSE_Address_P> m_P;
		const char* node_value;
	};
	struct SSP_BASE_EXPORT stuSCL_Communication_SubNetwork_ConnectedAP_GSE_Address_P
	{
		const char* type;
		const char* node_value;
	};
	struct SSP_BASE_EXPORT stuSCL_Communication_SubNetwork_ConnectedAP_GSE_MinTime
	{
		const char* unit;
		const char* multiplier;
		const char* node_value;
	};
	struct SSP_BASE_EXPORT stuSCL_Communication_SubNetwork_ConnectedAP_GSE_MaxTime
	{
		const char* unit;
		const char* multiplier;
		const char* node_value;
	};
	struct SSP_BASE_EXPORT stuSCL_Communication_SubNetwork_ConnectedAP_SMV
	{
		const char* cbName;
		const char* ldInst;
		SConstPtrList<stuSCL_Communication_SubNetwork_ConnectedAP_SMV_Address> m_Address;
		const char* node_value;
	};
	struct SSP_BASE_EXPORT stuSCL_Communication_SubNetwork_ConnectedAP_SMV_Address
	{
		SConstPtrList<stuSCL_Communication_SubNetwork_ConnectedAP_SMV_Address_P> m_P;
		const char* node_value;
	};
	struct SSP_BASE_EXPORT stuSCL_Communication_SubNetwork_ConnectedAP_SMV_Address_P
	{
		const char* type;
		const char* node_value;
	};
	struct SSP_BASE_EXPORT stuSCL_IED
	{
		const char* manufacturer;
		const char* name;
		const char* type;
		const char* desc;
		const char* configVersion;
		SConstPtrList<stuSCL_IED_Private> m_Private;
		SConstPtrList<stuSCL_IED_Services> m_Services;
		SConstPtrList<stuSCL_IED_AccessPoint> m_AccessPoint;
		const char* node_value;
	};
	struct SSP_BASE_EXPORT stuSCL_IED_Private
	{
		const char* type;
		const char* source;
		const char* node_value;
	};
	struct SSP_BASE_EXPORT stuSCL_IED_Services
	{
		SConstPtrList<stuSCL_IED_Services_DynAssociation> m_DynAssociation;
		SConstPtrList<stuSCL_IED_Services_SettingGroups> m_SettingGroups;
		SConstPtrList<stuSCL_IED_Services_GetDirectory> m_GetDirectory;
		SConstPtrList<stuSCL_IED_Services_GetDataObjectDefinition> m_GetDataObjectDefinition;
		SConstPtrList<stuSCL_IED_Services_DataObjectDirectory> m_DataObjectDirectory;
		SConstPtrList<stuSCL_IED_Services_GetDataSetValue> m_GetDataSetValue;
		SConstPtrList<stuSCL_IED_Services_DataSetDirectory> m_DataSetDirectory;
		SConstPtrList<stuSCL_IED_Services_ConfDataSet> m_ConfDataSet;
		SConstPtrList<stuSCL_IED_Services_ReadWrite> m_ReadWrite;
		SConstPtrList<stuSCL_IED_Services_ConfReportControl> m_ConfReportControl;
		SConstPtrList<stuSCL_IED_Services_GetCBValues> m_GetCBValues;
		SConstPtrList<stuSCL_IED_Services_ReportSettings> m_ReportSettings;
		SConstPtrList<stuSCL_IED_Services_GSESettings> m_GSESettings;
		SConstPtrList<stuSCL_IED_Services_GOOSE> m_GOOSE;
		SConstPtrList<stuSCL_IED_Services_FileHandling> m_FileHandling;
		SConstPtrList<stuSCL_IED_Services_ConfLNs> m_ConfLNs;
		SConstPtrList<stuSCL_IED_Services_SMVSettings> m_SMVSettings;
		SConstPtrList<stuSCL_IED_Services_SetDataSetValue> m_SetDataSetValue;
		SConstPtrList<stuSCL_IED_Services_ConfLogControl> m_ConfLogControl;
		SConstPtrList<stuSCL_IED_Services_LogSettings> m_LogSettings;
		SConstPtrList<stuSCL_IED_Services_TimerActivatedControl> m_TimerActivatedControl;
		const char* node_value;
	};
	struct SSP_BASE_EXPORT stuSCL_IED_Services_DynAssociation
	{
		const char* node_value;
	};
	struct SSP_BASE_EXPORT stuSCL_IED_Services_SettingGroups
	{
		SConstPtrList<stuSCL_IED_Services_SettingGroups_SGEdit> m_SGEdit;
		SConstPtrList<stuSCL_IED_Services_SettingGroups_ConfSG> m_ConfSG;
		const char* node_value;
	};
	struct SSP_BASE_EXPORT stuSCL_IED_Services_SettingGroups_SGEdit
	{
		const char* node_value;
	};
	struct SSP_BASE_EXPORT stuSCL_IED_Services_SettingGroups_ConfSG
	{
		const char* node_value;
	};
	struct SSP_BASE_EXPORT stuSCL_IED_Services_GetDirectory
	{
		const char* node_value;
	};
	struct SSP_BASE_EXPORT stuSCL_IED_Services_GetDataObjectDefinition
	{
		const char* node_value;
	};
	struct SSP_BASE_EXPORT stuSCL_IED_Services_DataObjectDirectory
	{
		const char* node_value;
	};
	struct SSP_BASE_EXPORT stuSCL_IED_Services_GetDataSetValue
	{
		const char* node_value;
	};
	struct SSP_BASE_EXPORT stuSCL_IED_Services_DataSetDirectory
	{
		const char* node_value;
	};
	struct SSP_BASE_EXPORT stuSCL_IED_Services_ConfDataSet
	{
		const char* max;
		const char* maxAttributes;
		const char* node_value;
	};
	struct SSP_BASE_EXPORT stuSCL_IED_Services_ReadWrite
	{
		const char* node_value;
	};
	struct SSP_BASE_EXPORT stuSCL_IED_Services_ConfReportControl
	{
		const char* max;
		const char* node_value;
	};
	struct SSP_BASE_EXPORT stuSCL_IED_Services_GetCBValues
	{
		const char* node_value;
	};
	struct SSP_BASE_EXPORT stuSCL_IED_Services_ReportSettings
	{
		const char* cbName;
		const char* datSet;
		const char* rptID;
		const char* optFields;
		const char* bufTime;
		const char* trgOps;
		const char* intgPd;
		const char* node_value;
	};
	struct SSP_BASE_EXPORT stuSCL_IED_Services_GSESettings
	{
		const char* cbName;
		const char* datSet;
		const char* appID;
		const char* dataLabel;
		const char* node_value;
	};
	struct SSP_BASE_EXPORT stuSCL_IED_Services_GOOSE
	{
		const char* max;
		const char* node_value;
	};
	struct SSP_BASE_EXPORT stuSCL_IED_Services_FileHandling
	{
		const char* node_value;
	};
	struct SSP_BASE_EXPORT stuSCL_IED_Services_ConfLNs
	{
		const char* fixLnInst;
		const char* fixPrefix;
		const char* node_value;
	};
	struct SSP_BASE_EXPORT stuSCL_IED_Services_SMVSettings
	{
		const char* cbName;
		const char* datSet;
		const char* optFields;
		const char* smpRate;
		const char* svID;
		SConstPtrList<stuSCL_IED_Services_SMVSettings_SmpRate> m_SmpRate;
		const char* node_value;
	};
	struct SSP_BASE_EXPORT stuSCL_IED_Services_SMVSettings_SmpRate
	{
		const char* node_value;
	};
	struct SSP_BASE_EXPORT stuSCL_IED_Services_SetDataSetValue
	{
		const char* node_value;
	};
	struct SSP_BASE_EXPORT stuSCL_IED_Services_ConfLogControl
	{
		const char* max;
		const char* node_value;
	};
	struct SSP_BASE_EXPORT stuSCL_IED_Services_LogSettings
	{
		const char* cbName;
		const char* datSet;
		const char* intgPd;
		const char* logEna;
		const char* trgOps;
		const char* node_value;
	};
	struct SSP_BASE_EXPORT stuSCL_IED_Services_TimerActivatedControl
	{
		const char* node_value;
	};
	struct SSP_BASE_EXPORT stuSCL_IED_AccessPoint
	{
		const char* name;
		const char* router;
		const char* clock;
		const char* desc;
		SConstPtrList<stuSCL_IED_AccessPoint_Server> m_Server;
		const char* node_value;
	};
	struct SSP_BASE_EXPORT stuSCL_IED_AccessPoint_Server
	{
		const char* timeout;
		const char* desc;
		SConstPtrList<stuSCL_IED_AccessPoint_Server_Authentication> m_Authentication;
		SConstPtrList<stuSCL_IED_AccessPoint_Server_LDevice> m_LDevice;
		const char* node_value;
	};
	struct SSP_BASE_EXPORT stuSCL_IED_AccessPoint_Server_Authentication
	{
		const char* none;
		const char* certificate;
		const char* strong;
		const char* weak;
		const char* password;
		const char* node_value;
	};
	struct SSP_BASE_EXPORT stuSCL_IED_AccessPoint_Server_LDevice
	{
		const char* inst;
		const char* desc;
		SConstPtrList<stuSCL_IED_AccessPoint_Server_LDevice_LN0> m_LN0;
		SConstPtrList<stuSCL_IED_AccessPoint_Server_LDevice_LN> m_LN;
		const char* node_value;
	};
	struct SSP_BASE_EXPORT stuSCL_IED_AccessPoint_Server_LDevice_LN0
	{
		const char* lnType;
		const char* lnClass;
		const char* inst;
		const char* desc;
		SConstPtrList<stuSCL_IED_AccessPoint_Server_LDevice_LN0_DataSet> m_DataSet;
		SConstPtrList<stuSCL_IED_AccessPoint_Server_LDevice_LN0_ReportControl> m_ReportControl;
		SConstPtrList<stuSCL_IED_AccessPoint_Server_LDevice_LN0_DOI> m_DOI;
		SConstPtrList<stuSCL_IED_AccessPoint_Server_LDevice_LN0_SettingControl> m_SettingControl;
		SConstPtrList<stuSCL_IED_AccessPoint_Server_LDevice_LN0_Inputs> m_Inputs;
		SConstPtrList<stuSCL_IED_AccessPoint_Server_LDevice_LN0_GSEControl> m_GSEControl;
		SConstPtrList<stuSCL_IED_AccessPoint_Server_LDevice_LN0_LogControl> m_LogControl;
		SConstPtrList<stuSCL_IED_AccessPoint_Server_LDevice_LN0_SampledValueControl> m_SampledValueControl;
		const char* node_value;
	};
	struct SSP_BASE_EXPORT stuSCL_IED_AccessPoint_Server_LDevice_LN0_DataSet
	{
		const char* desc;
		const char* name;
		SConstPtrList<stuSCL_IED_AccessPoint_Server_LDevice_LN0_DataSet_FCDA> m_FCDA;
		const char* node_value;
	};
	struct SSP_BASE_EXPORT stuSCL_IED_AccessPoint_Server_LDevice_LN0_DataSet_FCDA
	{
		const char* ldInst;
		const char* prefix;
		const char* lnClass;
		const char* doName;
		const char* fc;
		const char* daName;
		const char* lnInst;
		const char* node_value;
	};
	struct SSP_BASE_EXPORT stuSCL_IED_AccessPoint_Server_LDevice_LN0_ReportControl
	{
		const char* name;
		const char* datSet;
		const char* intgPd;
		const char* rptID;
		const char* confRev;
		const char* buffered;
		const char* bufTime;
		const char* desc;
		SConstPtrList<stuSCL_IED_AccessPoint_Server_LDevice_LN0_ReportControl_TrgOps> m_TrgOps;
		SConstPtrList<stuSCL_IED_AccessPoint_Server_LDevice_LN0_ReportControl_OptFields> m_OptFields;
		SConstPtrList<stuSCL_IED_AccessPoint_Server_LDevice_LN0_ReportControl_RptEnabled> m_RptEnabled;
		SConstPtrList<stuSCL_IED_AccessPoint_Server_LDevice_LN0_ReportControl_Private> m_Private;
		const char* node_value;
	};
	struct SSP_BASE_EXPORT stuSCL_IED_AccessPoint_Server_LDevice_LN0_ReportControl_TrgOps
	{
		const char* qchg;
		const char* period;
		const char* dupd;
		const char* dchg;
		const char* node_value;
	};
	struct SSP_BASE_EXPORT stuSCL_IED_AccessPoint_Server_LDevice_LN0_ReportControl_OptFields
	{
		const char* dataRef;
		const char* reasonCode;
		const char* configRef;
		const char* dataSet;
		const char* entryID;
		const char* timeStamp;
		const char* seqNum;
		const char* segmentation;
		const char* bufOvfl;
		const char* node_value;
	};
	struct SSP_BASE_EXPORT stuSCL_IED_AccessPoint_Server_LDevice_LN0_ReportControl_RptEnabled
	{
		const char* max;
		const char* node_value;
	};
	struct SSP_BASE_EXPORT stuSCL_IED_AccessPoint_Server_LDevice_LN0_ReportControl_Private
	{
		const char* sznari;
		const char* ScanRate;
		const char* node_value;
	};
	struct SSP_BASE_EXPORT stuSCL_IED_AccessPoint_Server_LDevice_LN0_DOI
	{
		const char* name;
		const char* desc;
		SConstPtrList<stuSCL_IED_AccessPoint_Server_LDevice_LN0_DOI_DAI> m_DAI;
		SConstPtrList<stuSCL_IED_AccessPoint_Server_LDevice_LN0_DOI_SDI> m_SDI;
		const char* node_value;
	};
	struct SSP_BASE_EXPORT stuSCL_IED_AccessPoint_Server_LDevice_LN0_DOI_DAI
	{
		const char* name;
		const char* valKind;
		const char* sAddr;
		const char* desc;
		SConstPtrList<stuSCL_IED_AccessPoint_Server_LDevice_LN0_DOI_DAI_Val> m_Val;
		const char* node_value;
	};
	struct SSP_BASE_EXPORT stuSCL_IED_AccessPoint_Server_LDevice_LN0_DOI_DAI_Val
	{
		const char* node_value;
	};
	struct SSP_BASE_EXPORT stuSCL_IED_AccessPoint_Server_LDevice_LN0_DOI_SDI
	{
		const char* name;
		SConstPtrList<stuSCL_IED_AccessPoint_Server_LDevice_LN0_DOI_SDI_DAI> m_DAI;
		SConstPtrList<stuSCL_IED_AccessPoint_Server_LDevice_LN0_DOI_SDI_SDI> m_SDI;
		const char* node_value;
	};
	struct SSP_BASE_EXPORT stuSCL_IED_AccessPoint_Server_LDevice_LN0_DOI_SDI_DAI
	{
		const char* name;
		const char* valKind;
		const char* sAddr;
		SConstPtrList<stuSCL_IED_AccessPoint_Server_LDevice_LN0_DOI_SDI_DAI_Val> m_Val;
		const char* node_value;
	};
	struct SSP_BASE_EXPORT stuSCL_IED_AccessPoint_Server_LDevice_LN0_DOI_SDI_DAI_Val
	{
		const char* node_value;
	};
	struct SSP_BASE_EXPORT stuSCL_IED_AccessPoint_Server_LDevice_LN0_DOI_SDI_SDI
	{
		const char* name;
		SConstPtrList<stuSCL_IED_AccessPoint_Server_LDevice_LN0_DOI_SDI_SDI_DAI> m_DAI;
		const char* node_value;
	};
	struct SSP_BASE_EXPORT stuSCL_IED_AccessPoint_Server_LDevice_LN0_DOI_SDI_SDI_DAI
	{
		const char* name;
		const char* valKind;
		SConstPtrList<stuSCL_IED_AccessPoint_Server_LDevice_LN0_DOI_SDI_SDI_DAI_Val> m_Val;
		const char* node_value;
	};
	struct SSP_BASE_EXPORT stuSCL_IED_AccessPoint_Server_LDevice_LN0_DOI_SDI_SDI_DAI_Val
	{
		const char* node_value;
	};
	struct SSP_BASE_EXPORT stuSCL_IED_AccessPoint_Server_LDevice_LN0_SettingControl
	{
		const char* numOfSGs;
		const char* actSG;
		const char* node_value;
	};
	struct SSP_BASE_EXPORT stuSCL_IED_AccessPoint_Server_LDevice_LN0_Inputs
	{
		SConstPtrList<stuSCL_IED_AccessPoint_Server_LDevice_LN0_Inputs_Private> m_Private;
		SConstPtrList<stuSCL_IED_AccessPoint_Server_LDevice_LN0_Inputs_ExtRef> m_ExtRef;
		const char* node_value;
	};
	struct SSP_BASE_EXPORT stuSCL_IED_AccessPoint_Server_LDevice_LN0_Inputs_Private
	{
		const char* node_value;
	};
	struct SSP_BASE_EXPORT stuSCL_IED_AccessPoint_Server_LDevice_LN0_Inputs_ExtRef
	{
		const char* iedName;
		const char* prefix;
		const char* doName;
		const char* lnInst;
		const char* lnClass;
		const char* daName;
		const char* intAddr;
		const char* ldInst;
		const char* node_value;
	};
	struct SSP_BASE_EXPORT stuSCL_IED_AccessPoint_Server_LDevice_LN0_GSEControl
	{
		const char* appID;
		const char* datSet;
		const char* confRev;
		const char* name;
		const char* type;
		const char* desc;
		SConstPtrList<stuSCL_IED_AccessPoint_Server_LDevice_LN0_GSEControl_Private> m_Private;
		const char* node_value;
	};
	struct SSP_BASE_EXPORT stuSCL_IED_AccessPoint_Server_LDevice_LN0_GSEControl_Private
	{
		const char* type;
		SConstPtrList<stuSCL_IED_AccessPoint_Server_LDevice_LN0_GSEControl_Private_P> m_P;
		const char* node_value;
	};
	struct SSP_BASE_EXPORT stuSCL_IED_AccessPoint_Server_LDevice_LN0_GSEControl_Private_P
	{
		const char* sznari;
		const char* type;
		const char* node_value;
	};
	struct SSP_BASE_EXPORT stuSCL_IED_AccessPoint_Server_LDevice_LN0_LogControl
	{
		const char* name;
		const char* desc;
		const char* datSet;
		const char* intgPd;
		const char* logName;
		const char* logEna;
		const char* reasonCode;
		SConstPtrList<stuSCL_IED_AccessPoint_Server_LDevice_LN0_LogControl_TrgOps> m_TrgOps;
		const char* node_value;
	};
	struct SSP_BASE_EXPORT stuSCL_IED_AccessPoint_Server_LDevice_LN0_LogControl_TrgOps
	{
		const char* qchg;
		const char* period;
		const char* dupd;
		const char* dchg;
		const char* node_value;
	};
	struct SSP_BASE_EXPORT stuSCL_IED_AccessPoint_Server_LDevice_LN0_SampledValueControl
	{
		const char* name;
		const char* datSet;
		const char* confRev;
		const char* nofASDU;
		const char* smpRate;
		const char* smvID;
		const char* multicast;
		const char* desc;
		SConstPtrList<stuSCL_IED_AccessPoint_Server_LDevice_LN0_SampledValueControl_SmvOpts> m_SmvOpts;
		const char* node_value;
	};
	struct SSP_BASE_EXPORT stuSCL_IED_AccessPoint_Server_LDevice_LN0_SampledValueControl_SmvOpts
	{
		const char* refreshTime;
		const char* sampleRate;
		const char* sampleSynchronized;
		const char* security;
		const char* dataRef;
		const char* node_value;
	};
	struct SSP_BASE_EXPORT stuSCL_IED_AccessPoint_Server_LDevice_LN
	{
		const char* lnType;
		const char* lnClass;
		const char* prefix;
		const char* inst;
		const char* desc;
		SConstPtrList<stuSCL_IED_AccessPoint_Server_LDevice_LN_DOI> m_DOI;
		const char* node_value;
	};
	struct SSP_BASE_EXPORT stuSCL_IED_AccessPoint_Server_LDevice_LN_DOI
	{
		const char* name;
		const char* desc;
		SConstPtrList<stuSCL_IED_AccessPoint_Server_LDevice_LN_DOI_SDI> m_SDI;
		SConstPtrList<stuSCL_IED_AccessPoint_Server_LDevice_LN_DOI_DAI> m_DAI;
		const char* node_value;
	};
	struct SSP_BASE_EXPORT stuSCL_IED_AccessPoint_Server_LDevice_LN_DOI_SDI
	{
		const char* name;
		const char* desc;
		SConstPtrList<stuSCL_IED_AccessPoint_Server_LDevice_LN_DOI_SDI_SDI> m_SDI;
		SConstPtrList<stuSCL_IED_AccessPoint_Server_LDevice_LN_DOI_SDI_DAI> m_DAI;
		const char* node_value;
	};
	struct SSP_BASE_EXPORT stuSCL_IED_AccessPoint_Server_LDevice_LN_DOI_SDI_SDI
	{
		const char* name;
		SConstPtrList<stuSCL_IED_AccessPoint_Server_LDevice_LN_DOI_SDI_SDI_SDI> m_SDI;
		SConstPtrList<stuSCL_IED_AccessPoint_Server_LDevice_LN_DOI_SDI_SDI_DAI> m_DAI;
		const char* node_value;
	};
	struct SSP_BASE_EXPORT stuSCL_IED_AccessPoint_Server_LDevice_LN_DOI_SDI_SDI_SDI
	{
		const char* name;
		SConstPtrList<stuSCL_IED_AccessPoint_Server_LDevice_LN_DOI_SDI_SDI_SDI_DAI> m_DAI;
		const char* node_value;
	};
	struct SSP_BASE_EXPORT stuSCL_IED_AccessPoint_Server_LDevice_LN_DOI_SDI_SDI_SDI_DAI
	{
		const char* name;
		const char* valKind;
		const char* sAddr;
		const char* desc;
		SConstPtrList<stuSCL_IED_AccessPoint_Server_LDevice_LN_DOI_SDI_SDI_SDI_DAI_Val> m_Val;
		const char* node_value;
	};
	struct SSP_BASE_EXPORT stuSCL_IED_AccessPoint_Server_LDevice_LN_DOI_SDI_SDI_SDI_DAI_Val
	{
		const char* node_value;
	};
	struct SSP_BASE_EXPORT stuSCL_IED_AccessPoint_Server_LDevice_LN_DOI_SDI_SDI_DAI
	{
		const char* name;
		const char* valKind;
		const char* sAddr;
		SConstPtrList<stuSCL_IED_AccessPoint_Server_LDevice_LN_DOI_SDI_SDI_DAI_Val> m_Val;
		const char* node_value;
	};
	struct SSP_BASE_EXPORT stuSCL_IED_AccessPoint_Server_LDevice_LN_DOI_SDI_SDI_DAI_Val
	{
		const char* node_value;
	};
	struct SSP_BASE_EXPORT stuSCL_IED_AccessPoint_Server_LDevice_LN_DOI_SDI_DAI
	{
		const char* name;
		const char* valKind;
		const char* sAddr;
		const char* desc;
		SConstPtrList<stuSCL_IED_AccessPoint_Server_LDevice_LN_DOI_SDI_DAI_Val> m_Val;
		const char* node_value;
	};
	struct SSP_BASE_EXPORT stuSCL_IED_AccessPoint_Server_LDevice_LN_DOI_SDI_DAI_Val
	{
		const char* node_value;
	};
	struct SSP_BASE_EXPORT stuSCL_IED_AccessPoint_Server_LDevice_LN_DOI_DAI
	{
		const char* name;
		const char* valKind;
		const char* sAddr;
		const char* desc;
		SConstPtrList<stuSCL_IED_AccessPoint_Server_LDevice_LN_DOI_DAI_Val> m_Val;
		const char* node_value;
	};
	struct SSP_BASE_EXPORT stuSCL_IED_AccessPoint_Server_LDevice_LN_DOI_DAI_Val
	{
		const char* sGroup;
		const char* node_value;
	};
	struct SSP_BASE_EXPORT stuSCL_DataTypeTemplates
	{
		SConstPtrList<stuSCL_DataTypeTemplates_LNodeType> m_LNodeType;
		SConstPtrList<stuSCL_DataTypeTemplates_DOType> m_DOType;
		SConstPtrList<stuSCL_DataTypeTemplates_DAType> m_DAType;
		SConstPtrList<stuSCL_DataTypeTemplates_EnumType> m_EnumType;
		const char* node_value;
	};
	struct SSP_BASE_EXPORT stuSCL_DataTypeTemplates_LNodeType
	{
		const char* lnClass;
		const char* id;
		const char* desc;
		SConstPtrList<stuSCL_DataTypeTemplates_LNodeType_DO> m_DO;
		const char* node_value;
	};
	struct SSP_BASE_EXPORT stuSCL_DataTypeTemplates_LNodeType_DO
	{
		const char* name;
		const char* type;
		const char* desc;
		const char* transient;
		const char* ext;
		const char* eOption;
		const char* node_value;
	};
	struct SSP_BASE_EXPORT stuSCL_DataTypeTemplates_DOType
	{
		const char* id;
		const char* cdc;
		const char* desc;
		SConstPtrList<stuSCL_DataTypeTemplates_DOType_DA> m_DA;
		SConstPtrList<stuSCL_DataTypeTemplates_DOType_SDO> m_SDO;
		const char* node_value;
	};
	struct SSP_BASE_EXPORT stuSCL_DataTypeTemplates_DOType_DA
	{
		const char* name;
		const char* bType;
		const char* type;
		const char* fc;
		const char* dchg;
		const char* qchg;
		const char* dupd;
		SConstPtrList<stuSCL_DataTypeTemplates_DOType_DA_Val> m_Val;
		const char* node_value;
	};
	struct SSP_BASE_EXPORT stuSCL_DataTypeTemplates_DOType_DA_Val
	{
		const char* node_value;
	};
	struct SSP_BASE_EXPORT stuSCL_DataTypeTemplates_DOType_SDO
	{
		const char* name;
		const char* type;
		const char* node_value;
	};
	struct SSP_BASE_EXPORT stuSCL_DataTypeTemplates_DAType
	{
		const char* id;
		SConstPtrList<stuSCL_DataTypeTemplates_DAType_BDA> m_BDA;
		const char* node_value;
	};
	struct SSP_BASE_EXPORT stuSCL_DataTypeTemplates_DAType_BDA
	{
		const char* name;
		const char* bType;
		const char* type;
		const char* valKind;
		const char* count;
		const char* node_value;
	};
	struct SSP_BASE_EXPORT stuSCL_DataTypeTemplates_EnumType
	{
		const char* id;
		SConstPtrList<stuSCL_DataTypeTemplates_EnumType_EnumVal> m_EnumVal;
		const char* node_value;
	};
	struct SSP_BASE_EXPORT stuSCL_DataTypeTemplates_EnumType_EnumVal
	{
		const char* ord;
		const char* node_value;
	};
	CSpSclParser();
	virtual ~CSpSclParser();
	static stuSaxChoice* CB_SCL(void* cbParam, SSaxAttribute *pAttribute,int iAttributeCount,char* sNodeValue);
	static stuSaxChoice* CB_SCL_Private(void* cbParam, SSaxAttribute *pAttribute,int iAttributeCount,char* sNodeValue);
	static stuSaxChoice* CB_SCL_Header(void* cbParam, SSaxAttribute *pAttribute,int iAttributeCount,char* sNodeValue);
	static stuSaxChoice* CB_SCL_Header_History(void* cbParam, SSaxAttribute *pAttribute,int iAttributeCount,char* sNodeValue);
	static stuSaxChoice* CB_SCL_Header_History_Hitem(void* cbParam, SSaxAttribute *pAttribute,int iAttributeCount,char* sNodeValue);
	static stuSaxChoice* CB_SCL_Substation(void* cbParam, SSaxAttribute *pAttribute,int iAttributeCount,char* sNodeValue);
	static stuSaxChoice* CB_SCL_Substation_VoltageLevel(void* cbParam, SSaxAttribute *pAttribute,int iAttributeCount,char* sNodeValue);
	static stuSaxChoice* CB_SCL_Substation_VoltageLevel_Bay(void* cbParam, SSaxAttribute *pAttribute,int iAttributeCount,char* sNodeValue);
	static stuSaxChoice* CB_SCL_Communication(void* cbParam, SSaxAttribute *pAttribute,int iAttributeCount,char* sNodeValue);
	static stuSaxChoice* CB_SCL_Communication_SubNetwork(void* cbParam, SSaxAttribute *pAttribute,int iAttributeCount,char* sNodeValue);
	static stuSaxChoice* CB_SCL_Communication_SubNetwork_BitRate(void* cbParam, SSaxAttribute *pAttribute,int iAttributeCount,char* sNodeValue);
	static stuSaxChoice* CB_SCL_Communication_SubNetwork_ConnectedAP(void* cbParam, SSaxAttribute *pAttribute,int iAttributeCount,char* sNodeValue);
	static stuSaxChoice* CB_SCL_Communication_SubNetwork_ConnectedAP_Private(void* cbParam, SSaxAttribute *pAttribute,int iAttributeCount,char* sNodeValue);
	static stuSaxChoice* CB_SCL_Communication_SubNetwork_ConnectedAP_Address(void* cbParam, SSaxAttribute *pAttribute,int iAttributeCount,char* sNodeValue);
	static stuSaxChoice* CB_SCL_Communication_SubNetwork_ConnectedAP_Address_P(void* cbParam, SSaxAttribute *pAttribute,int iAttributeCount,char* sNodeValue);
	static stuSaxChoice* CB_SCL_Communication_SubNetwork_ConnectedAP_GSE(void* cbParam, SSaxAttribute *pAttribute,int iAttributeCount,char* sNodeValue);
	static stuSaxChoice* CB_SCL_Communication_SubNetwork_ConnectedAP_GSE_Address(void* cbParam, SSaxAttribute *pAttribute,int iAttributeCount,char* sNodeValue);
	static stuSaxChoice* CB_SCL_Communication_SubNetwork_ConnectedAP_GSE_Address_P(void* cbParam, SSaxAttribute *pAttribute,int iAttributeCount,char* sNodeValue);
	static stuSaxChoice* CB_SCL_Communication_SubNetwork_ConnectedAP_GSE_MinTime(void* cbParam, SSaxAttribute *pAttribute,int iAttributeCount,char* sNodeValue);
	static stuSaxChoice* CB_SCL_Communication_SubNetwork_ConnectedAP_GSE_MaxTime(void* cbParam, SSaxAttribute *pAttribute,int iAttributeCount,char* sNodeValue);
	static stuSaxChoice* CB_SCL_Communication_SubNetwork_ConnectedAP_SMV(void* cbParam, SSaxAttribute *pAttribute,int iAttributeCount,char* sNodeValue);
	static stuSaxChoice* CB_SCL_Communication_SubNetwork_ConnectedAP_SMV_Address(void* cbParam, SSaxAttribute *pAttribute,int iAttributeCount,char* sNodeValue);
	static stuSaxChoice* CB_SCL_Communication_SubNetwork_ConnectedAP_SMV_Address_P(void* cbParam, SSaxAttribute *pAttribute,int iAttributeCount,char* sNodeValue);
	static stuSaxChoice* CB_SCL_IED(void* cbParam, SSaxAttribute *pAttribute,int iAttributeCount,char* sNodeValue);
	static stuSaxChoice* CB_SCL_IED_Private(void* cbParam, SSaxAttribute *pAttribute,int iAttributeCount,char* sNodeValue);
	static stuSaxChoice* CB_SCL_IED_Services(void* cbParam, SSaxAttribute *pAttribute,int iAttributeCount,char* sNodeValue);
	static stuSaxChoice* CB_SCL_IED_Services_DynAssociation(void* cbParam, SSaxAttribute *pAttribute,int iAttributeCount,char* sNodeValue);
	static stuSaxChoice* CB_SCL_IED_Services_SettingGroups(void* cbParam, SSaxAttribute *pAttribute,int iAttributeCount,char* sNodeValue);
	static stuSaxChoice* CB_SCL_IED_Services_SettingGroups_SGEdit(void* cbParam, SSaxAttribute *pAttribute,int iAttributeCount,char* sNodeValue);
	static stuSaxChoice* CB_SCL_IED_Services_SettingGroups_ConfSG(void* cbParam, SSaxAttribute *pAttribute,int iAttributeCount,char* sNodeValue);
	static stuSaxChoice* CB_SCL_IED_Services_GetDirectory(void* cbParam, SSaxAttribute *pAttribute,int iAttributeCount,char* sNodeValue);
	static stuSaxChoice* CB_SCL_IED_Services_GetDataObjectDefinition(void* cbParam, SSaxAttribute *pAttribute,int iAttributeCount,char* sNodeValue);
	static stuSaxChoice* CB_SCL_IED_Services_DataObjectDirectory(void* cbParam, SSaxAttribute *pAttribute,int iAttributeCount,char* sNodeValue);
	static stuSaxChoice* CB_SCL_IED_Services_GetDataSetValue(void* cbParam, SSaxAttribute *pAttribute,int iAttributeCount,char* sNodeValue);
	static stuSaxChoice* CB_SCL_IED_Services_DataSetDirectory(void* cbParam, SSaxAttribute *pAttribute,int iAttributeCount,char* sNodeValue);
	static stuSaxChoice* CB_SCL_IED_Services_ConfDataSet(void* cbParam, SSaxAttribute *pAttribute,int iAttributeCount,char* sNodeValue);
	static stuSaxChoice* CB_SCL_IED_Services_ReadWrite(void* cbParam, SSaxAttribute *pAttribute,int iAttributeCount,char* sNodeValue);
	static stuSaxChoice* CB_SCL_IED_Services_ConfReportControl(void* cbParam, SSaxAttribute *pAttribute,int iAttributeCount,char* sNodeValue);
	static stuSaxChoice* CB_SCL_IED_Services_GetCBValues(void* cbParam, SSaxAttribute *pAttribute,int iAttributeCount,char* sNodeValue);
	static stuSaxChoice* CB_SCL_IED_Services_ReportSettings(void* cbParam, SSaxAttribute *pAttribute,int iAttributeCount,char* sNodeValue);
	static stuSaxChoice* CB_SCL_IED_Services_GSESettings(void* cbParam, SSaxAttribute *pAttribute,int iAttributeCount,char* sNodeValue);
	static stuSaxChoice* CB_SCL_IED_Services_GOOSE(void* cbParam, SSaxAttribute *pAttribute,int iAttributeCount,char* sNodeValue);
	static stuSaxChoice* CB_SCL_IED_Services_FileHandling(void* cbParam, SSaxAttribute *pAttribute,int iAttributeCount,char* sNodeValue);
	static stuSaxChoice* CB_SCL_IED_Services_ConfLNs(void* cbParam, SSaxAttribute *pAttribute,int iAttributeCount,char* sNodeValue);
	static stuSaxChoice* CB_SCL_IED_Services_SMVSettings(void* cbParam, SSaxAttribute *pAttribute,int iAttributeCount,char* sNodeValue);
	static stuSaxChoice* CB_SCL_IED_Services_SMVSettings_SmpRate(void* cbParam, SSaxAttribute *pAttribute,int iAttributeCount,char* sNodeValue);
	static stuSaxChoice* CB_SCL_IED_Services_SetDataSetValue(void* cbParam, SSaxAttribute *pAttribute,int iAttributeCount,char* sNodeValue);
	static stuSaxChoice* CB_SCL_IED_Services_ConfLogControl(void* cbParam, SSaxAttribute *pAttribute,int iAttributeCount,char* sNodeValue);
	static stuSaxChoice* CB_SCL_IED_Services_LogSettings(void* cbParam, SSaxAttribute *pAttribute,int iAttributeCount,char* sNodeValue);
	static stuSaxChoice* CB_SCL_IED_Services_TimerActivatedControl(void* cbParam, SSaxAttribute *pAttribute,int iAttributeCount,char* sNodeValue);
	static stuSaxChoice* CB_SCL_IED_AccessPoint(void* cbParam, SSaxAttribute *pAttribute,int iAttributeCount,char* sNodeValue);
	static stuSaxChoice* CB_SCL_IED_AccessPoint_Server(void* cbParam, SSaxAttribute *pAttribute,int iAttributeCount,char* sNodeValue);
	static stuSaxChoice* CB_SCL_IED_AccessPoint_Server_Authentication(void* cbParam, SSaxAttribute *pAttribute,int iAttributeCount,char* sNodeValue);
	static stuSaxChoice* CB_SCL_IED_AccessPoint_Server_LDevice(void* cbParam, SSaxAttribute *pAttribute,int iAttributeCount,char* sNodeValue);
	static stuSaxChoice* CB_SCL_IED_AccessPoint_Server_LDevice_LN0(void* cbParam, SSaxAttribute *pAttribute,int iAttributeCount,char* sNodeValue);
	static stuSaxChoice* CB_SCL_IED_AccessPoint_Server_LDevice_LN0_DataSet(void* cbParam, SSaxAttribute *pAttribute,int iAttributeCount,char* sNodeValue);
	static stuSaxChoice* CB_SCL_IED_AccessPoint_Server_LDevice_LN0_DataSet_FCDA(void* cbParam, SSaxAttribute *pAttribute,int iAttributeCount,char* sNodeValue);
	static stuSaxChoice* CB_SCL_IED_AccessPoint_Server_LDevice_LN0_ReportControl(void* cbParam, SSaxAttribute *pAttribute,int iAttributeCount,char* sNodeValue);
	static stuSaxChoice* CB_SCL_IED_AccessPoint_Server_LDevice_LN0_ReportControl_TrgOps(void* cbParam, SSaxAttribute *pAttribute,int iAttributeCount,char* sNodeValue);
	static stuSaxChoice* CB_SCL_IED_AccessPoint_Server_LDevice_LN0_ReportControl_OptFields(void* cbParam, SSaxAttribute *pAttribute,int iAttributeCount,char* sNodeValue);
	static stuSaxChoice* CB_SCL_IED_AccessPoint_Server_LDevice_LN0_ReportControl_RptEnabled(void* cbParam, SSaxAttribute *pAttribute,int iAttributeCount,char* sNodeValue);
	static stuSaxChoice* CB_SCL_IED_AccessPoint_Server_LDevice_LN0_ReportControl_Private(void* cbParam, SSaxAttribute *pAttribute,int iAttributeCount,char* sNodeValue);
	static stuSaxChoice* CB_SCL_IED_AccessPoint_Server_LDevice_LN0_DOI(void* cbParam, SSaxAttribute *pAttribute,int iAttributeCount,char* sNodeValue);
	static stuSaxChoice* CB_SCL_IED_AccessPoint_Server_LDevice_LN0_DOI_DAI(void* cbParam, SSaxAttribute *pAttribute,int iAttributeCount,char* sNodeValue);
	static stuSaxChoice* CB_SCL_IED_AccessPoint_Server_LDevice_LN0_DOI_DAI_Val(void* cbParam, SSaxAttribute *pAttribute,int iAttributeCount,char* sNodeValue);
	static stuSaxChoice* CB_SCL_IED_AccessPoint_Server_LDevice_LN0_DOI_SDI(void* cbParam, SSaxAttribute *pAttribute,int iAttributeCount,char* sNodeValue);
	static stuSaxChoice* CB_SCL_IED_AccessPoint_Server_LDevice_LN0_DOI_SDI_DAI(void* cbParam, SSaxAttribute *pAttribute,int iAttributeCount,char* sNodeValue);
	static stuSaxChoice* CB_SCL_IED_AccessPoint_Server_LDevice_LN0_DOI_SDI_DAI_Val(void* cbParam, SSaxAttribute *pAttribute,int iAttributeCount,char* sNodeValue);
	static stuSaxChoice* CB_SCL_IED_AccessPoint_Server_LDevice_LN0_DOI_SDI_SDI(void* cbParam, SSaxAttribute *pAttribute,int iAttributeCount,char* sNodeValue);
	static stuSaxChoice* CB_SCL_IED_AccessPoint_Server_LDevice_LN0_DOI_SDI_SDI_DAI(void* cbParam, SSaxAttribute *pAttribute,int iAttributeCount,char* sNodeValue);
	static stuSaxChoice* CB_SCL_IED_AccessPoint_Server_LDevice_LN0_DOI_SDI_SDI_DAI_Val(void* cbParam, SSaxAttribute *pAttribute,int iAttributeCount,char* sNodeValue);
	static stuSaxChoice* CB_SCL_IED_AccessPoint_Server_LDevice_LN0_SettingControl(void* cbParam, SSaxAttribute *pAttribute,int iAttributeCount,char* sNodeValue);
	static stuSaxChoice* CB_SCL_IED_AccessPoint_Server_LDevice_LN0_Inputs(void* cbParam, SSaxAttribute *pAttribute,int iAttributeCount,char* sNodeValue);
	static stuSaxChoice* CB_SCL_IED_AccessPoint_Server_LDevice_LN0_Inputs_Private(void* cbParam, SSaxAttribute *pAttribute,int iAttributeCount,char* sNodeValue);
	static stuSaxChoice* CB_SCL_IED_AccessPoint_Server_LDevice_LN0_Inputs_ExtRef(void* cbParam, SSaxAttribute *pAttribute,int iAttributeCount,char* sNodeValue);
	static stuSaxChoice* CB_SCL_IED_AccessPoint_Server_LDevice_LN0_GSEControl(void* cbParam, SSaxAttribute *pAttribute,int iAttributeCount,char* sNodeValue);
	static stuSaxChoice* CB_SCL_IED_AccessPoint_Server_LDevice_LN0_GSEControl_Private(void* cbParam, SSaxAttribute *pAttribute,int iAttributeCount,char* sNodeValue);
	static stuSaxChoice* CB_SCL_IED_AccessPoint_Server_LDevice_LN0_GSEControl_Private_P(void* cbParam, SSaxAttribute *pAttribute,int iAttributeCount,char* sNodeValue);
	static stuSaxChoice* CB_SCL_IED_AccessPoint_Server_LDevice_LN0_LogControl(void* cbParam, SSaxAttribute *pAttribute,int iAttributeCount,char* sNodeValue);
	static stuSaxChoice* CB_SCL_IED_AccessPoint_Server_LDevice_LN0_LogControl_TrgOps(void* cbParam, SSaxAttribute *pAttribute,int iAttributeCount,char* sNodeValue);
	static stuSaxChoice* CB_SCL_IED_AccessPoint_Server_LDevice_LN0_SampledValueControl(void* cbParam, SSaxAttribute *pAttribute,int iAttributeCount,char* sNodeValue);
	static stuSaxChoice* CB_SCL_IED_AccessPoint_Server_LDevice_LN0_SampledValueControl_SmvOpts(void* cbParam, SSaxAttribute *pAttribute,int iAttributeCount,char* sNodeValue);
	static stuSaxChoice* CB_SCL_IED_AccessPoint_Server_LDevice_LN(void* cbParam, SSaxAttribute *pAttribute,int iAttributeCount,char* sNodeValue);
	static stuSaxChoice* CB_SCL_IED_AccessPoint_Server_LDevice_LN_DOI(void* cbParam, SSaxAttribute *pAttribute,int iAttributeCount,char* sNodeValue);
	static stuSaxChoice* CB_SCL_IED_AccessPoint_Server_LDevice_LN_DOI_SDI(void* cbParam, SSaxAttribute *pAttribute,int iAttributeCount,char* sNodeValue);
	static stuSaxChoice* CB_SCL_IED_AccessPoint_Server_LDevice_LN_DOI_SDI_SDI(void* cbParam, SSaxAttribute *pAttribute,int iAttributeCount,char* sNodeValue);
	static stuSaxChoice* CB_SCL_IED_AccessPoint_Server_LDevice_LN_DOI_SDI_SDI_SDI(void* cbParam, SSaxAttribute *pAttribute,int iAttributeCount,char* sNodeValue);
	static stuSaxChoice* CB_SCL_IED_AccessPoint_Server_LDevice_LN_DOI_SDI_SDI_SDI_DAI(void* cbParam, SSaxAttribute *pAttribute,int iAttributeCount,char* sNodeValue);
	static stuSaxChoice* CB_SCL_IED_AccessPoint_Server_LDevice_LN_DOI_SDI_SDI_SDI_DAI_Val(void* cbParam, SSaxAttribute *pAttribute,int iAttributeCount,char* sNodeValue);
	static stuSaxChoice* CB_SCL_IED_AccessPoint_Server_LDevice_LN_DOI_SDI_SDI_DAI(void* cbParam, SSaxAttribute *pAttribute,int iAttributeCount,char* sNodeValue);
	static stuSaxChoice* CB_SCL_IED_AccessPoint_Server_LDevice_LN_DOI_SDI_SDI_DAI_Val(void* cbParam, SSaxAttribute *pAttribute,int iAttributeCount,char* sNodeValue);
	static stuSaxChoice* CB_SCL_IED_AccessPoint_Server_LDevice_LN_DOI_SDI_DAI(void* cbParam, SSaxAttribute *pAttribute,int iAttributeCount,char* sNodeValue);
	static stuSaxChoice* CB_SCL_IED_AccessPoint_Server_LDevice_LN_DOI_SDI_DAI_Val(void* cbParam, SSaxAttribute *pAttribute,int iAttributeCount,char* sNodeValue);
	static stuSaxChoice* CB_SCL_IED_AccessPoint_Server_LDevice_LN_DOI_DAI(void* cbParam, SSaxAttribute *pAttribute,int iAttributeCount,char* sNodeValue);
	static stuSaxChoice* CB_SCL_IED_AccessPoint_Server_LDevice_LN_DOI_DAI_Val(void* cbParam, SSaxAttribute *pAttribute,int iAttributeCount,char* sNodeValue);
	static stuSaxChoice* CB_SCL_DataTypeTemplates(void* cbParam, SSaxAttribute *pAttribute,int iAttributeCount,char* sNodeValue);
	static stuSaxChoice* CB_SCL_DataTypeTemplates_LNodeType(void* cbParam, SSaxAttribute *pAttribute,int iAttributeCount,char* sNodeValue);
	static stuSaxChoice* CB_SCL_DataTypeTemplates_LNodeType_DO(void* cbParam, SSaxAttribute *pAttribute,int iAttributeCount,char* sNodeValue);
	static stuSaxChoice* CB_SCL_DataTypeTemplates_DOType(void* cbParam, SSaxAttribute *pAttribute,int iAttributeCount,char* sNodeValue);
	static stuSaxChoice* CB_SCL_DataTypeTemplates_DOType_DA(void* cbParam, SSaxAttribute *pAttribute,int iAttributeCount,char* sNodeValue);
	static stuSaxChoice* CB_SCL_DataTypeTemplates_DOType_DA_Val(void* cbParam, SSaxAttribute *pAttribute,int iAttributeCount,char* sNodeValue);
	static stuSaxChoice* CB_SCL_DataTypeTemplates_DOType_SDO(void* cbParam, SSaxAttribute *pAttribute,int iAttributeCount,char* sNodeValue);
	static stuSaxChoice* CB_SCL_DataTypeTemplates_DAType(void* cbParam, SSaxAttribute *pAttribute,int iAttributeCount,char* sNodeValue);
	static stuSaxChoice* CB_SCL_DataTypeTemplates_DAType_BDA(void* cbParam, SSaxAttribute *pAttribute,int iAttributeCount,char* sNodeValue);
	static stuSaxChoice* CB_SCL_DataTypeTemplates_EnumType(void* cbParam, SSaxAttribute *pAttribute,int iAttributeCount,char* sNodeValue);
	static stuSaxChoice* CB_SCL_DataTypeTemplates_EnumType_EnumVal(void* cbParam, SSaxAttribute *pAttribute,int iAttributeCount,char* sNodeValue);
	SConstPtrList<stuSCL_Private> m_Private;
	SConstPtrList<stuSCL_Header> m_Header;
	SConstPtrList<stuSCL_Substation> m_Substation;
	SConstPtrList<stuSCL_Communication> m_Communication;
	SConstPtrList<stuSCL_IED> m_IED;
	SConstPtrList<stuSCL_DataTypeTemplates> m_DataTypeTemplates;
private:
	stuSCL_Private* m_p_SCL_Private;
	stuSCL_Header* m_p_SCL_Header;
	stuSCL_Header_History* m_p_SCL_Header_History;
	stuSCL_Header_History_Hitem* m_p_SCL_Header_History_Hitem;
	stuSCL_Substation* m_p_SCL_Substation;
	stuSCL_Substation_VoltageLevel* m_p_SCL_Substation_VoltageLevel;
	stuSCL_Substation_VoltageLevel_Bay* m_p_SCL_Substation_VoltageLevel_Bay;
	stuSCL_Communication* m_p_SCL_Communication;
	stuSCL_Communication_SubNetwork* m_p_SCL_Communication_SubNetwork;
	stuSCL_Communication_SubNetwork_BitRate* m_p_SCL_Communication_SubNetwork_BitRate;
	stuSCL_Communication_SubNetwork_ConnectedAP* m_p_SCL_Communication_SubNetwork_ConnectedAP;
	stuSCL_Communication_SubNetwork_ConnectedAP_Private* m_p_SCL_Communication_SubNetwork_ConnectedAP_Private;
	stuSCL_Communication_SubNetwork_ConnectedAP_Address* m_p_SCL_Communication_SubNetwork_ConnectedAP_Address;
	stuSCL_Communication_SubNetwork_ConnectedAP_Address_P* m_p_SCL_Communication_SubNetwork_ConnectedAP_Address_P;
	stuSCL_Communication_SubNetwork_ConnectedAP_GSE* m_p_SCL_Communication_SubNetwork_ConnectedAP_GSE;
	stuSCL_Communication_SubNetwork_ConnectedAP_GSE_Address* m_p_SCL_Communication_SubNetwork_ConnectedAP_GSE_Address;
	stuSCL_Communication_SubNetwork_ConnectedAP_GSE_Address_P* m_p_SCL_Communication_SubNetwork_ConnectedAP_GSE_Address_P;
	stuSCL_Communication_SubNetwork_ConnectedAP_GSE_MinTime* m_p_SCL_Communication_SubNetwork_ConnectedAP_GSE_MinTime;
	stuSCL_Communication_SubNetwork_ConnectedAP_GSE_MaxTime* m_p_SCL_Communication_SubNetwork_ConnectedAP_GSE_MaxTime;
	stuSCL_Communication_SubNetwork_ConnectedAP_SMV* m_p_SCL_Communication_SubNetwork_ConnectedAP_SMV;
	stuSCL_Communication_SubNetwork_ConnectedAP_SMV_Address* m_p_SCL_Communication_SubNetwork_ConnectedAP_SMV_Address;
	stuSCL_Communication_SubNetwork_ConnectedAP_SMV_Address_P* m_p_SCL_Communication_SubNetwork_ConnectedAP_SMV_Address_P;
	stuSCL_IED* m_p_SCL_IED;
	stuSCL_IED_Private* m_p_SCL_IED_Private;
	stuSCL_IED_Services* m_p_SCL_IED_Services;
	stuSCL_IED_Services_DynAssociation* m_p_SCL_IED_Services_DynAssociation;
	stuSCL_IED_Services_SettingGroups* m_p_SCL_IED_Services_SettingGroups;
	stuSCL_IED_Services_SettingGroups_SGEdit* m_p_SCL_IED_Services_SettingGroups_SGEdit;
	stuSCL_IED_Services_SettingGroups_ConfSG* m_p_SCL_IED_Services_SettingGroups_ConfSG;
	stuSCL_IED_Services_GetDirectory* m_p_SCL_IED_Services_GetDirectory;
	stuSCL_IED_Services_GetDataObjectDefinition* m_p_SCL_IED_Services_GetDataObjectDefinition;
	stuSCL_IED_Services_DataObjectDirectory* m_p_SCL_IED_Services_DataObjectDirectory;
	stuSCL_IED_Services_GetDataSetValue* m_p_SCL_IED_Services_GetDataSetValue;
	stuSCL_IED_Services_DataSetDirectory* m_p_SCL_IED_Services_DataSetDirectory;
	stuSCL_IED_Services_ConfDataSet* m_p_SCL_IED_Services_ConfDataSet;
	stuSCL_IED_Services_ReadWrite* m_p_SCL_IED_Services_ReadWrite;
	stuSCL_IED_Services_ConfReportControl* m_p_SCL_IED_Services_ConfReportControl;
	stuSCL_IED_Services_GetCBValues* m_p_SCL_IED_Services_GetCBValues;
	stuSCL_IED_Services_ReportSettings* m_p_SCL_IED_Services_ReportSettings;
	stuSCL_IED_Services_GSESettings* m_p_SCL_IED_Services_GSESettings;
	stuSCL_IED_Services_GOOSE* m_p_SCL_IED_Services_GOOSE;
	stuSCL_IED_Services_FileHandling* m_p_SCL_IED_Services_FileHandling;
	stuSCL_IED_Services_ConfLNs* m_p_SCL_IED_Services_ConfLNs;
	stuSCL_IED_Services_SMVSettings* m_p_SCL_IED_Services_SMVSettings;
	stuSCL_IED_Services_SMVSettings_SmpRate* m_p_SCL_IED_Services_SMVSettings_SmpRate;
	stuSCL_IED_Services_SetDataSetValue* m_p_SCL_IED_Services_SetDataSetValue;
	stuSCL_IED_Services_ConfLogControl* m_p_SCL_IED_Services_ConfLogControl;
	stuSCL_IED_Services_LogSettings* m_p_SCL_IED_Services_LogSettings;
	stuSCL_IED_Services_TimerActivatedControl* m_p_SCL_IED_Services_TimerActivatedControl;
	stuSCL_IED_AccessPoint* m_p_SCL_IED_AccessPoint;
	stuSCL_IED_AccessPoint_Server* m_p_SCL_IED_AccessPoint_Server;
	stuSCL_IED_AccessPoint_Server_Authentication* m_p_SCL_IED_AccessPoint_Server_Authentication;
	stuSCL_IED_AccessPoint_Server_LDevice* m_p_SCL_IED_AccessPoint_Server_LDevice;
	stuSCL_IED_AccessPoint_Server_LDevice_LN0* m_p_SCL_IED_AccessPoint_Server_LDevice_LN0;
	stuSCL_IED_AccessPoint_Server_LDevice_LN0_DataSet* m_p_SCL_IED_AccessPoint_Server_LDevice_LN0_DataSet;
	stuSCL_IED_AccessPoint_Server_LDevice_LN0_DataSet_FCDA* m_p_SCL_IED_AccessPoint_Server_LDevice_LN0_DataSet_FCDA;
	stuSCL_IED_AccessPoint_Server_LDevice_LN0_ReportControl* m_p_SCL_IED_AccessPoint_Server_LDevice_LN0_ReportControl;
	stuSCL_IED_AccessPoint_Server_LDevice_LN0_ReportControl_TrgOps* m_p_SCL_IED_AccessPoint_Server_LDevice_LN0_ReportControl_TrgOps;
	stuSCL_IED_AccessPoint_Server_LDevice_LN0_ReportControl_OptFields* m_p_SCL_IED_AccessPoint_Server_LDevice_LN0_ReportControl_OptFields;
	stuSCL_IED_AccessPoint_Server_LDevice_LN0_ReportControl_RptEnabled* m_p_SCL_IED_AccessPoint_Server_LDevice_LN0_ReportControl_RptEnabled;
	stuSCL_IED_AccessPoint_Server_LDevice_LN0_ReportControl_Private* m_p_SCL_IED_AccessPoint_Server_LDevice_LN0_ReportControl_Private;
	stuSCL_IED_AccessPoint_Server_LDevice_LN0_DOI* m_p_SCL_IED_AccessPoint_Server_LDevice_LN0_DOI;
	stuSCL_IED_AccessPoint_Server_LDevice_LN0_DOI_DAI* m_p_SCL_IED_AccessPoint_Server_LDevice_LN0_DOI_DAI;
	stuSCL_IED_AccessPoint_Server_LDevice_LN0_DOI_DAI_Val* m_p_SCL_IED_AccessPoint_Server_LDevice_LN0_DOI_DAI_Val;
	stuSCL_IED_AccessPoint_Server_LDevice_LN0_DOI_SDI* m_p_SCL_IED_AccessPoint_Server_LDevice_LN0_DOI_SDI;
	stuSCL_IED_AccessPoint_Server_LDevice_LN0_DOI_SDI_DAI* m_p_SCL_IED_AccessPoint_Server_LDevice_LN0_DOI_SDI_DAI;
	stuSCL_IED_AccessPoint_Server_LDevice_LN0_DOI_SDI_DAI_Val* m_p_SCL_IED_AccessPoint_Server_LDevice_LN0_DOI_SDI_DAI_Val;
	stuSCL_IED_AccessPoint_Server_LDevice_LN0_DOI_SDI_SDI* m_p_SCL_IED_AccessPoint_Server_LDevice_LN0_DOI_SDI_SDI;
	stuSCL_IED_AccessPoint_Server_LDevice_LN0_DOI_SDI_SDI_DAI* m_p_SCL_IED_AccessPoint_Server_LDevice_LN0_DOI_SDI_SDI_DAI;
	stuSCL_IED_AccessPoint_Server_LDevice_LN0_DOI_SDI_SDI_DAI_Val* m_p_SCL_IED_AccessPoint_Server_LDevice_LN0_DOI_SDI_SDI_DAI_Val;
	stuSCL_IED_AccessPoint_Server_LDevice_LN0_SettingControl* m_p_SCL_IED_AccessPoint_Server_LDevice_LN0_SettingControl;
	stuSCL_IED_AccessPoint_Server_LDevice_LN0_Inputs* m_p_SCL_IED_AccessPoint_Server_LDevice_LN0_Inputs;
	stuSCL_IED_AccessPoint_Server_LDevice_LN0_Inputs_Private* m_p_SCL_IED_AccessPoint_Server_LDevice_LN0_Inputs_Private;
	stuSCL_IED_AccessPoint_Server_LDevice_LN0_Inputs_ExtRef* m_p_SCL_IED_AccessPoint_Server_LDevice_LN0_Inputs_ExtRef;
	stuSCL_IED_AccessPoint_Server_LDevice_LN0_GSEControl* m_p_SCL_IED_AccessPoint_Server_LDevice_LN0_GSEControl;
	stuSCL_IED_AccessPoint_Server_LDevice_LN0_GSEControl_Private* m_p_SCL_IED_AccessPoint_Server_LDevice_LN0_GSEControl_Private;
	stuSCL_IED_AccessPoint_Server_LDevice_LN0_GSEControl_Private_P* m_p_SCL_IED_AccessPoint_Server_LDevice_LN0_GSEControl_Private_P;
	stuSCL_IED_AccessPoint_Server_LDevice_LN0_LogControl* m_p_SCL_IED_AccessPoint_Server_LDevice_LN0_LogControl;
	stuSCL_IED_AccessPoint_Server_LDevice_LN0_LogControl_TrgOps* m_p_SCL_IED_AccessPoint_Server_LDevice_LN0_LogControl_TrgOps;
	stuSCL_IED_AccessPoint_Server_LDevice_LN0_SampledValueControl* m_p_SCL_IED_AccessPoint_Server_LDevice_LN0_SampledValueControl;
	stuSCL_IED_AccessPoint_Server_LDevice_LN0_SampledValueControl_SmvOpts* m_p_SCL_IED_AccessPoint_Server_LDevice_LN0_SampledValueControl_SmvOpts;
	stuSCL_IED_AccessPoint_Server_LDevice_LN* m_p_SCL_IED_AccessPoint_Server_LDevice_LN;
	stuSCL_IED_AccessPoint_Server_LDevice_LN_DOI* m_p_SCL_IED_AccessPoint_Server_LDevice_LN_DOI;
	stuSCL_IED_AccessPoint_Server_LDevice_LN_DOI_SDI* m_p_SCL_IED_AccessPoint_Server_LDevice_LN_DOI_SDI;
	stuSCL_IED_AccessPoint_Server_LDevice_LN_DOI_SDI_SDI* m_p_SCL_IED_AccessPoint_Server_LDevice_LN_DOI_SDI_SDI;
	stuSCL_IED_AccessPoint_Server_LDevice_LN_DOI_SDI_SDI_SDI* m_p_SCL_IED_AccessPoint_Server_LDevice_LN_DOI_SDI_SDI_SDI;
	stuSCL_IED_AccessPoint_Server_LDevice_LN_DOI_SDI_SDI_SDI_DAI* m_p_SCL_IED_AccessPoint_Server_LDevice_LN_DOI_SDI_SDI_SDI_DAI;
	stuSCL_IED_AccessPoint_Server_LDevice_LN_DOI_SDI_SDI_SDI_DAI_Val* m_p_SCL_IED_AccessPoint_Server_LDevice_LN_DOI_SDI_SDI_SDI_DAI_Val;
	stuSCL_IED_AccessPoint_Server_LDevice_LN_DOI_SDI_SDI_DAI* m_p_SCL_IED_AccessPoint_Server_LDevice_LN_DOI_SDI_SDI_DAI;
	stuSCL_IED_AccessPoint_Server_LDevice_LN_DOI_SDI_SDI_DAI_Val* m_p_SCL_IED_AccessPoint_Server_LDevice_LN_DOI_SDI_SDI_DAI_Val;
	stuSCL_IED_AccessPoint_Server_LDevice_LN_DOI_SDI_DAI* m_p_SCL_IED_AccessPoint_Server_LDevice_LN_DOI_SDI_DAI;
	stuSCL_IED_AccessPoint_Server_LDevice_LN_DOI_SDI_DAI_Val* m_p_SCL_IED_AccessPoint_Server_LDevice_LN_DOI_SDI_DAI_Val;
	stuSCL_IED_AccessPoint_Server_LDevice_LN_DOI_DAI* m_p_SCL_IED_AccessPoint_Server_LDevice_LN_DOI_DAI;
	stuSCL_IED_AccessPoint_Server_LDevice_LN_DOI_DAI_Val* m_p_SCL_IED_AccessPoint_Server_LDevice_LN_DOI_DAI_Val;
	stuSCL_DataTypeTemplates* m_p_SCL_DataTypeTemplates;
	stuSCL_DataTypeTemplates_LNodeType* m_p_SCL_DataTypeTemplates_LNodeType;
	stuSCL_DataTypeTemplates_LNodeType_DO* m_p_SCL_DataTypeTemplates_LNodeType_DO;
	stuSCL_DataTypeTemplates_DOType* m_p_SCL_DataTypeTemplates_DOType;
	stuSCL_DataTypeTemplates_DOType_DA* m_p_SCL_DataTypeTemplates_DOType_DA;
	stuSCL_DataTypeTemplates_DOType_DA_Val* m_p_SCL_DataTypeTemplates_DOType_DA_Val;
	stuSCL_DataTypeTemplates_DOType_SDO* m_p_SCL_DataTypeTemplates_DOType_SDO;
	stuSCL_DataTypeTemplates_DAType* m_p_SCL_DataTypeTemplates_DAType;
	stuSCL_DataTypeTemplates_DAType_BDA* m_p_SCL_DataTypeTemplates_DAType_BDA;
	stuSCL_DataTypeTemplates_EnumType* m_p_SCL_DataTypeTemplates_EnumType;
	stuSCL_DataTypeTemplates_EnumType_EnumVal* m_p_SCL_DataTypeTemplates_EnumType_EnumVal;

};

class SSP_BASE_EXPORT CSpSclParserEx : public CSpSclParser
{
public:
	struct SSP_BASE_EXPORT CLNodeType;
	struct SSP_BASE_EXPORT CLNodeType_DO;
	struct SSP_BASE_EXPORT CDOType;
	struct SSP_BASE_EXPORT CDOType_DA;
	struct SSP_BASE_EXPORT CDOType_SDO;
	struct SSP_BASE_EXPORT CDAType;
	struct SSP_BASE_EXPORT CDAType_BDA;

	struct SSP_BASE_EXPORT CLNodeType
	{
		stuSCL_DataTypeTemplates_LNodeType *pLNodeType;
		SPtrList<CLNodeType_DO> m_DO;
		CLNodeType()
		{
			m_DO.setAutoDelete(true);
		}
		~CLNodeType()
		{
			m_DO.clear();
		}
		CDOType* SearchDOTypeByName(const char* do_name)
		{
			unsigned long pos;
			CLNodeType_DO* pDO = m_DO.FetchFirst(pos);
			while(pDO)
			{
				if(SString::equals(pDO->pDO->name,do_name))
					return pDO->m_pDoType;
				pDO = m_DO.FetchNext(pos);
			}
			return NULL;
		}
	};
	struct SSP_BASE_EXPORT CLNodeType_DO
	{
		stuSCL_DataTypeTemplates_LNodeType_DO *pDO;
		CDOType *m_pDoType;
	};
	struct SSP_BASE_EXPORT CDOType
	{
		stuSCL_DataTypeTemplates_DOType *pDOType;
		SPtrList<CDOType_SDO> m_SDO;
		SPtrList<CDOType_DA> m_DA;
		CDOType()
		{
			m_SDO.setAutoDelete(true);
			m_DA.setAutoDelete(true);
		}
		~CDOType()
		{
			m_SDO.clear();
			m_DA.clear();
		}
		CDOType* SearchDOTypeByName(const char* do_name)
		{
			unsigned long pos;
			CDOType_SDO* pDO = m_SDO.FetchFirst(pos);
			while(pDO)
			{
				if(SString::equals(pDO->pSDO->name,do_name))
					return pDO->m_pDOType;
				pDO = m_SDO.FetchNext(pos);
			}
			return NULL;
		}
	};
	struct SSP_BASE_EXPORT CDOType_DA
	{
		stuSCL_DataTypeTemplates_DOType_DA *pDA;
		const char* da_val;
		CDAType *m_pDAType;
		stuSCL_DataTypeTemplates_EnumType* m_pEnumType;
	};
	struct SSP_BASE_EXPORT CDOType_SDO
	{
		stuSCL_DataTypeTemplates_DOType_SDO *pSDO;
		CDOType *m_pDOType;
	};
	struct SSP_BASE_EXPORT CDAType
	{
		stuSCL_DataTypeTemplates_DAType *pDAType;
		SPtrList<CDAType_BDA> m_DAType_BDA;
		CDAType()
		{
			m_DAType_BDA.setAutoDelete(true);
		}
		~CDAType()
		{
			m_DAType_BDA.clear();
		}
	};
	struct SSP_BASE_EXPORT CDAType_BDA
	{
		stuSCL_DataTypeTemplates_DAType_BDA *pDAType_BDA;
		CDAType* m_pDAType;
		stuSCL_DataTypeTemplates_EnumType* m_pEnumType;

		SPtrList<stuSCL_DataTypeTemplates_DAType_BDA> m_BDA;
		CDAType_BDA()
		{
			m_BDA.setAutoDelete(true);
		}
		~CDAType_BDA()
		{
			m_BDA.clear();
		}
	};
	SPtrList<CLNodeType> m_LNodeTypes;
	SPtrList<CDOType> m_DOTypes;
	SPtrList<CDAType> m_DATypes;
	SPtrList<stuSCL_DataTypeTemplates_EnumType> m_EnumTypes;
	map<std::string,CLNodeType*> m_mapLNodeTypes;
	map<std::string,CDOType*> m_mapDOTypes;
	map<std::string,CDAType*> m_mapDATypes;
	map<std::string,stuSCL_DataTypeTemplates_EnumType*> m_mapEnumTypes;
	stuSCL_DataTypeTemplates_EnumType *m_pEnumType;
	stuSCL_DataTypeTemplates_EnumType *m_pmultiplier;

	CSpSclParserEx()
	{
		m_LNodeTypes.setAutoDelete(true);
		m_DOTypes.setAutoDelete(true);
		m_DATypes.setAutoDelete(true);
		m_EnumTypes.setAutoDelete(false);
		m_pEnumType = NULL;
		m_pmultiplier = NULL;
	};
	virtual ~CSpSclParserEx()
	{
		m_LNodeTypes.clear();
		m_DOTypes.clear();
		m_DATypes.clear();
		m_EnumTypes.clear();
	};

	void Clear()
	{
		m_Private.clear();
		m_Header.clear();
		m_Substation.clear();
		m_Communication.clear();
		m_IED.clear();
		m_DataTypeTemplates.clear();
		m_ConstString.Clear();

		m_LNodeTypes.clear();
		m_DOTypes.clear();
		m_DATypes.clear();
		m_EnumTypes.clear();
		m_mapLNodeTypes.clear();
		m_mapDOTypes.clear();
		m_mapDATypes.clear();
		m_mapEnumTypes.clear();

		m_pEnumType = NULL;
		m_pmultiplier = NULL;
	}

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  处理所有的数据类型模板
	// 作    者:  邵凯田
	// 创建时间:  2015-12-25 14:41
	// 参数说明:  void
	// 返 回 值:  void
	//////////////////////////////////////////////////////////////////////////
	void ProcessDataTypeTemplates();

	bool ProcessLNodeType(CLNodeType *pDTMLNodeType);
	bool ProcessDOType(CDOType *pDTMDOType);
	bool ProcessDAType(CDAType *pDTMDAType);


	//////////////////////////////////////////////////////////////////////////
	// 描    述:  查找SCL模型中的LNodeType节点
	// 作    者:  邵凯田
	// 创建时间:  2015-12-25 14:35
	// 参数说明:  @lnodetype
	// 返 回 值:  stuSCL_DataTypeTemplates_LNodeType*
	//////////////////////////////////////////////////////////////////////////
	stuSCL_DataTypeTemplates_LNodeType* SearchSclLNodeType(char* lnodetype);

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  查找DOType节点
	// 作    者:  邵凯田
	// 创建时间:  2015-12-25 14:36
	// 参数说明:  @
	// 返 回 值:  stuSCL_DataTypeTemplates_DOType*
	//////////////////////////////////////////////////////////////////////////
	stuSCL_DataTypeTemplates_DOType* SearchSclDOType(char* dotype);

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  查找DAType
	// 作    者:  邵凯田
	// 创建时间:  2015-12-25 14:37
	// 参数说明:  
	// 返 回 值:  stuSCL_DataTypeTemplates_DAType*
	//////////////////////////////////////////////////////////////////////////
	stuSCL_DataTypeTemplates_DAType* SearchSclDAType(char* datype);

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  查找EnumType
	// 作    者:  邵凯田
	// 创建时间:  2015-12-25 14:37
	// 参数说明:  
	// 返 回 值:  stuSCL_DataTypeTemplates_EnumType*
	//////////////////////////////////////////////////////////////////////////
	stuSCL_DataTypeTemplates_EnumType* SearchSclEnumType(char* enumtype);

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  检查处理后的LNodeType
	// 作    者:  邵凯田
	// 创建时间:  2015-12-25 14:37
	// 参数说明:  
	// 返 回 值:  CLNodeType*
	//////////////////////////////////////////////////////////////////////////
	CLNodeType* SearchDTMLNodeType(char* lnodetype);

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  新建LNodeType对象
	// 作    者:  邵凯田
	// 创建时间:  2015-12-25 14:38
	// 参数说明:  
	// 返 回 值:  CLNodeType*
	//////////////////////////////////////////////////////////////////////////
	CLNodeType* NewDTMLNodeType(stuSCL_DataTypeTemplates_LNodeType *plnt);

	CDOType* SearchDTMDOType(char *id);
	CDAType* SearchDTMDAType(char *id);

	CDOType* NewDTMDOType(stuSCL_DataTypeTemplates_DOType *pDOType);
	CDAType* NewDTMDAType(stuSCL_DataTypeTemplates_DAType *pDAType);

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  根据IED名称查找IED指针
	// 作    者:  邵凯田
	// 创建时间:  2015-12-23 16:31
	// 参数说明:  @ied_name
	// 返 回 值:  stuSCL_IED*, NULL表示未找到
	//////////////////////////////////////////////////////////////////////////
	stuSCL_IED* SearchIedByName(char *ied_name);

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  根据LD实例名，从当前IED中查找指定的LD
	// 作    者:  邵凯田
	// 创建时间:  2015-12-23 17:12
	// 参数说明:  @pIed，ldInst
	// 返 回 值:  stuSCL_IED_AccessPoint_Server_LDevice*，NULL表示失败
	//////////////////////////////////////////////////////////////////////////
	stuSCL_IED_AccessPoint_Server_LDevice* SearchLDByInst(stuSCL_IED *pIed,char* ldInst);

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  提取IED对应的所有LD
	// 作    者:  邵凯田
	// 创建时间:  2015-12-23 16:31
	// 参数说明:  
	// 返 回 值:  返回LD数量
	//////////////////////////////////////////////////////////////////////////
	int RetrieveIedLd(stuSCL_IED* pIed,SPtrList<stuSCL_IED_AccessPoint_Server_LDevice> &lds);

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  查找指定条件的LN
	// 作    者:  邵凯田
	// 创建时间:  2015-12-23 17:08
	// 参数说明:  @pThisLD表示当前LD，优先从当前LD中找该LN
	// 返 回 值:  stuSCL_IED_AccessPoint_Server_LDevice_LN*， NULL表示找不到
	//////////////////////////////////////////////////////////////////////////
	stuSCL_IED_AccessPoint_Server_LDevice_LN* SearchLn(stuSCL_IED *pIed,
		stuSCL_IED_AccessPoint_Server_LDevice* &pThisLD,
		char* ldInst,char* prefix,char* lnClass,char* lnInst);

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  根据节点路径查找指定IED的LN节点
	// 作    者:  邵凯田
	// 创建时间:  2015-12-24 16:08
	// 参数说明:  @pIed为对应IED
	//         :  @path为路径，不含IEDNAME，如PI/GOINGGIO1.SPCSO1.stVal
	// 返 回 值:  stuSCL_IED_AccessPoint_Server_LDevice_LN*, NULL表示找不到
	//////////////////////////////////////////////////////////////////////////
	stuSCL_IED_AccessPoint_Server_LDevice_LN_DOI* SearchDoiByPath(stuSCL_IED *pIed,
		const char* path);

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  在指定的LN中查找指定名称的DO
	// 作    者:  邵凯田
	// 创建时间:  2015-12-23 17:40
	// 参数说明:  @pLN
	// 返 回 值:  stuSCL_IED_AccessPoint_Server_LDevice_LN_DOI*， NULL表示找不到
	//////////////////////////////////////////////////////////////////////////
	stuSCL_IED_AccessPoint_Server_LDevice_LN_DOI* SearchDoiByName(stuSCL_IED_AccessPoint_Server_LDevice_LN *pLN,char* doName);
	stuSCL_IED_AccessPoint_Server_LDevice_LN0_DOI* SearchDoiByName(stuSCL_IED_AccessPoint_Server_LDevice_LN0 *pLN,char* doName);

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  根据数据集中的FCDA查找对应节点的描述信息
	// 作    者:  邵凯田
	// 创建时间:  2015-12-25 9:53
	// 参数说明:  
	// 返 回 值:  true/false
	//////////////////////////////////////////////////////////////////////////
	bool GetFcdaNodeInfo(stuSCL_IED_AccessPoint_Server_LDevice_LN0_DataSet_FCDA *pFCDA,
		stuSCL_IED *pIed,stuSCL_IED_AccessPoint_Server_LDevice* &pThisLD,
		char* &ln_desc, char* &do_desc, char* &da_desc, char* &dime,char* &ln_type,char* &do_type, SString *pExtAttr=NULL);

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  根据节点路径查找对应节点的描述信息
	// 作    者:  邵凯田
	// 创建时间:  2015-12-25 9:53
	// 参数说明:  
	// 返 回 值:  true/false
	//////////////////////////////////////////////////////////////////////////
	bool GetPathNodeInfo(char* path,stuSCL_IED *pIed,
		char* &ln_desc, char* &do_desc, char* &da_desc, char* &dime, SString *pExtAttr=NULL);

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  取指定DO中的DAI下的dU节点的Value子节点
	// 作    者:  邵凯田
	// 创建时间:  2015-12-23 19:11
	// 参数说明:  @pDoi
	// 返 回 值:  stuSCL_IED_AccessPoint_Server_LDevice_LN_DOI_DAI_Val*, NULL表示未找到
	//////////////////////////////////////////////////////////////////////////
	stuSCL_IED_AccessPoint_Server_LDevice_LN_DOI_DAI_Val* SearchDaiDu(stuSCL_IED_AccessPoint_Server_LDevice_LN_DOI *pDoi);
	stuSCL_IED_AccessPoint_Server_LDevice_LN0_DOI_DAI_Val* SearchDaiDu(stuSCL_IED_AccessPoint_Server_LDevice_LN0_DOI *pDoi);

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  取指定LD对应的访问点
	// 作    者:  邵凯田
	// 创建时间:  2015-12-23 19:33
	// 参数说明:  
	// 返 回 值:  stuSCL_IED_AccessPoint* , NULL表示找不到
	//////////////////////////////////////////////////////////////////////////
	stuSCL_IED_AccessPoint* SearchIedAccessPointByLd(stuSCL_IED *pIed,stuSCL_IED_AccessPoint_Server_LDevice *pThisLD);

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  取通讯段的访问点
	// 作    者:  邵凯田
	// 创建时间:  2015-12-23 20:42
	// 参数说明:  @ied_name,ap_name
	// 返 回 值:  stuSCL_Communication_SubNetwork_ConnectedAP*, NULL表示找不到
	//////////////////////////////////////////////////////////////////////////
	stuSCL_Communication_SubNetwork_ConnectedAP* SearchConnectAP(char* ied_name,char* ap_name);

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  取指定连接访问点的指定LD及控制块的GOOSE访问参数
	// 作    者:  邵凯田
	// 创建时间:  2015-12-23 20:59
	// 参数说明:  @pCAP
	// 返 回 值:  stuSCL_Communication_SubNetwork_ConnectedAP_GSE*, NULL表示找不到
	//////////////////////////////////////////////////////////////////////////
	stuSCL_Communication_SubNetwork_ConnectedAP_GSE* SearchCAPGSE(stuSCL_Communication_SubNetwork_ConnectedAP *pCAP,char* cbName,char* ldInst);

	// 描    述:  取指定连接访问点的指定LD及控制块的SMV访问参数
	// 作    者:  邵凯田
	// 创建时间:  2015-12-23 20:59
	// 参数说明:  @pCAP
	// 返 回 值:  stuSCL_Communication_SubNetwork_ConnectedAP_SMV*, NULL表示找不到
	//////////////////////////////////////////////////////////////////////////
	stuSCL_Communication_SubNetwork_ConnectedAP_SMV* SearchCAPSMV(stuSCL_Communication_SubNetwork_ConnectedAP *pCAP,char* cbName,char* ldInst);


	//////////////////////////////////////////////////////////////////////////
	// 描    述:  取指定GSE通讯参数的内容
	// 作    者:  邵凯田
	// 创建时间:  2015-12-23 21:28
	// 参数说明:  
	// 返 回 值:  void
	//////////////////////////////////////////////////////////////////////////
	void GetCAPGSE_Param(stuSCL_Communication_SubNetwork_ConnectedAP_GSE *pGSE,char* &mac, char* &appid, char* &vlan_priority, char* &vlan, char* &mintime, char* &maxtime);
	void GetCAPGSE_Param(char* ied_name,char* ap_name,char* cbName,char* ldInst,char* &mac, char* &appid, char* &vlan_priority, char* &vlan, char* &mintime, char* &maxtime);

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  取指定SMV通讯参数的内容
	// 作    者:  邵凯田
	// 创建时间:  2015-12-23 21:47
	// 参数说明:  
	// 返 回 值:  void
	//////////////////////////////////////////////////////////////////////////
	void GetCAPSMV_Param(stuSCL_Communication_SubNetwork_ConnectedAP_SMV *pSMV,char* &mac, char* &appid, char* &vlan_priority, char* &vlan);
	void GetCAPSMV_Param(char* ied_name,char* ap_name,char* cbName,char* ldInst,char* &mac, char* &appid, char* &vlan_priority, char* &vlan);

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  根据Inputs中的外部引用，查找对应的的虚端子连接点，并引用返回其所有的IED、LD、DATASET以及控制块
	// 作    者:  邵凯田
	// 创建时间:  2015-12-24 13:54
	// 参数说明:  
	// 返 回 值:  stuSCL_IED_AccessPoint_Server_LDevice_LN0_DataSet_FCDA*表示找不到
	//////////////////////////////////////////////////////////////////////////
	stuSCL_IED_AccessPoint_Server_LDevice_LN0_DataSet_FCDA* SearchFCDAByExtref(stuSCL_IED_AccessPoint_Server_LDevice_LN0_Inputs_ExtRef *pExtRef,
		stuSCL_IED* &pIed,stuSCL_IED_AccessPoint_Server_LDevice* &pLd,
		stuSCL_IED_AccessPoint_Server_LDevice_LN0_DataSet* &pDataSet,
		stuSCL_IED_AccessPoint_Server_LDevice_LN0_SampledValueControl* &pSvcb,
		stuSCL_IED_AccessPoint_Server_LDevice_LN0_GSEControl* &pGocb);

	char* SearchEnumVal(stuSCL_DataTypeTemplates_EnumType *pEnum,const char* ord );
	char* GetUnitDesc(const char* SIUnit,const char* multiplier);

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  取数据项的基础数据类型描述
	// 作    者:  邵凯田
	// 创建时间:  2016-1-6 14:43
	// 参数说明:  
	// 返 回 值:  char*
	//////////////////////////////////////////////////////////////////////////
	char* GetDaValType(const char* do_type,const char* do_name,const char* da_name);
};
#endif
