/**
 *
 * 文 件 名 : sp_virtual_terminal.cpp
 * 创建日期 : 2015-12-23 15:49
 * 作    者 : 邵凯田(skt001@163.com)
 * 修改日期 : $Date: $
 * 当前版本 : $Revision: $
 * 功能描述 : 虚端口相关定义及表现封装
 * 修改记录 : 
 *            $Log: $
 *
 * Ver  Date        Author  Comments
 * ---  ----------  ------  -------------------------------------------
 * 001	2015-12-23	邵凯田　创建文件
 *
 **/

#include "sp_virtual_terminal.h"
//#include "MUSimulator.h"
const char g_vt_string_null[1]={0};

//////////////////////////////////////////////////////////////////////////
// 描    述:  从SAX的SCL解析器中加载虚端子信息
// 作    者:  邵凯田
// 创建时间:  2015-12-23 16:18
// 参数说明:  pSclParser
// 返 回 值:  true/false
//////////////////////////////////////////////////////////////////////////
bool CSclVt::LoadBySclParser(CSpSclParserEx *pSclParser)
{
	if(pSclParser == NULL || pSclParser->m_IED.count() == 0)
		return false;
	Clear();
	unsigned long pos1=0,pos2=0;
	stuSclVtIed *pVtIed;
	CSpSclParserEx::stuSCL_IED *pIed = pSclParser->m_IED.FetchFirst(pos1);
	while(pIed)
	{
		pVtIed = new stuSclVtIed();
		pVtIed->name		= (char*)pIed->name;
		pVtIed->desc		= SString::safeChar(pIed->desc);
		pVtIed->type		= SString::safeChar(pIed->type);
		pVtIed->manufacturer= (char*)pIed->manufacturer;
		pVtIed->configVersion=(char*)pIed->configVersion;

		if(LoadIedBySclParser(pSclParser,pVtIed,pIed))
			m_Ied.append(pVtIed);
		else
			delete pVtIed;

		pIed = pSclParser->m_IED.FetchNext(pos1);
	}

	pIed = pSclParser->m_IED.FetchFirst(pos1);
	pVtIed = m_Ied.FetchFirst(pos2);
	while(pIed && pVtIed)
	{
		LoadIedInputsBySclParser(pSclParser,pVtIed,pIed);

		pIed = pSclParser->m_IED.FetchNext(pos1);
		pVtIed = m_Ied.FetchNext(pos2);
	}
	LoadCommunicationBySclParser(pSclParser);

	return true;
}

bool CSclVt::LoadCommunicationBySclParser(CSpSclParserEx *pSclParser)
{
	CSpSclParserEx::stuSCL_Communication *pComm = pSclParser->m_Communication[0];
	if(pComm == NULL)
		return false;
	unsigned long pos1=0,pos2=0,pos3=0,pos4=0;
	CSpSclParserEx::stuSCL_Communication_SubNetwork *pSubNetwork = pComm->m_SubNetwork.FetchFirst(pos1);
	while(pSubNetwork)
	{
		stuSclVtSubNetwork *pVtSubNetwork = new stuSclVtSubNetwork();
		pVtSubNetwork->desc = SString::safeChar(pSubNetwork->desc);
		pVtSubNetwork->name = SString::safeChar(pSubNetwork->name);
		pVtSubNetwork->type = SString::safeChar(pSubNetwork->type);
		m_SubNetwork.append(pVtSubNetwork);
		CSpSclParserEx::stuSCL_Communication_SubNetwork_ConnectedAP *pConnectedAP = pSubNetwork->m_ConnectedAP.FetchFirst(pos2);
		while(pConnectedAP)
		{
			stuSclVtSubNetworkConnectAP *pVtConnectAP = new stuSclVtSubNetworkConnectAP();
			pVtConnectAP->ied_name = SString::safeChar(pConnectedAP->iedName);
			pVtConnectAP->ap_name = SString::safeChar(pConnectedAP->apName);
			pVtConnectAP->desc = SString::safeChar(pConnectedAP->desc);
			pVtSubNetwork->m_ConnectAP.append(pVtConnectAP);

			CSpSclParserEx::stuSCL_Communication_SubNetwork_ConnectedAP_Address *pAddress = pConnectedAP->m_Address.FetchFirst(pos3);
			while(pAddress)
			{
				stuSclVtSubNetworkConnectAPAddress *pVtAddress = new stuSclVtSubNetworkConnectAPAddress();
				pVtAddress->ip = SString::safeChar(NULL);
				pVtAddress->ip_subnet = SString::safeChar(NULL);
				pVtAddress->ip_gateway = SString::safeChar(NULL);
				pVtConnectAP->m_Address.append(pVtAddress);

				CSpSclParserEx::stuSCL_Communication_SubNetwork_ConnectedAP_Address_P *p = pAddress->m_P.FetchFirst(pos4);
				while(p)
				{
					if(SString::equals(p->type,"IP"))
						pVtAddress->ip = p->node_value;
					else if(SString::equals(p->type,"IP-SUBNET"))
						pVtAddress->ip_subnet = p->node_value;
					else if(SString::equals(p->type,"IP-GATEWAY"))
						pVtAddress->ip_gateway = p->node_value;
					p = pAddress->m_P.FetchNext(pos4);
				}

				pAddress = pConnectedAP->m_Address.FetchNext(pos3);
			}

			CSpSclParserEx::stuSCL_Communication_SubNetwork_ConnectedAP_GSE *pGSE = pConnectedAP->m_GSE.FetchFirst(pos3);
			while(pGSE)
			{
				stuSclVtSubNetworkConnectAPGSE *pVtAddress = new stuSclVtSubNetworkConnectAPGSE();
				pVtAddress->cb_name = pGSE->cbName;
				pVtAddress->ld_inst = pGSE->ldInst;
				pVtAddress->mac_address = SString::safeChar(NULL);
				pVtAddress->appid = SString::safeChar(NULL);
				pVtAddress->vlan_priority = SString::safeChar(NULL);
				pVtAddress->vlan_id = SString::safeChar(NULL);
				pVtConnectAP->m_GSE.append(pVtAddress);

				CSpSclParserEx::stuSCL_Communication_SubNetwork_ConnectedAP_GSE_Address *pAddr = pGSE->m_Address[0];
				CSpSclParserEx::stuSCL_Communication_SubNetwork_ConnectedAP_GSE_Address_P *p = pAddr==NULL?NULL:pAddr->m_P.FetchFirst(pos4);
				while(p)
				{
					if(SString::equals(p->type,"MAC-Address"))
						pVtAddress->mac_address = p->node_value;
					else if(SString::equals(p->type,"APPID"))
						pVtAddress->appid = p->node_value;
					else if(SString::equals(p->type,"VLAN-PRIORITY"))
						pVtAddress->vlan_priority = p->node_value;
					else if(SString::equals(p->type,"VLAN-ID"))
						pVtAddress->vlan_id = p->node_value;
					p = pAddr->m_P.FetchNext(pos4);
				}

				pGSE = pConnectedAP->m_GSE.FetchNext(pos3);
			}

			CSpSclParserEx::stuSCL_Communication_SubNetwork_ConnectedAP_SMV *pSMV = pConnectedAP->m_SMV.FetchFirst(pos3);
			while(pSMV)
			{
				stuSclVtSubNetworkConnectAPSMV *pVtAddress = new stuSclVtSubNetworkConnectAPSMV();
				pVtAddress->cb_name = pSMV->cbName;
				pVtAddress->ld_inst = pSMV->ldInst;
				pVtAddress->mac_address = SString::safeChar(NULL);
				pVtAddress->appid = SString::safeChar(NULL);
				pVtAddress->vlan_priority = SString::safeChar(NULL);
				pVtAddress->vlan_id = SString::safeChar(NULL);
				pVtConnectAP->m_SMV.append(pVtAddress);

				CSpSclParserEx::stuSCL_Communication_SubNetwork_ConnectedAP_SMV_Address *pAddr = pSMV->m_Address[0];
				CSpSclParserEx::stuSCL_Communication_SubNetwork_ConnectedAP_SMV_Address_P *p = pAddr==NULL?NULL:pAddr->m_P.FetchFirst(pos4);
				while(p)
				{
					if(SString::equals(p->type,"MAC-Address"))
						pVtAddress->mac_address = p->node_value;
					else if(SString::equals(p->type,"APPID"))
						pVtAddress->appid = p->node_value;
					else if(SString::equals(p->type,"VLAN-PRIORITY"))
						pVtAddress->vlan_priority = p->node_value;
					else if(SString::equals(p->type,"VLAN-ID"))
						pVtAddress->vlan_id = p->node_value;
					p = pAddr->m_P.FetchNext(pos4);
				}

				pSMV = pConnectedAP->m_SMV.FetchNext(pos3);
			}

			pConnectedAP = pSubNetwork->m_ConnectedAP.FetchNext(pos2);
		}
		pSubNetwork = pComm->m_SubNetwork.FetchNext(pos1);
	}
	return true;
}

bool CSclVt::LoadIedBySclParser(CSpSclParserEx *pSclParser,stuSclVtIed *pVtIed,CSpSclParserEx::stuSCL_IED *pIed)
{
	unsigned long pos1=0,pos2=0,pos3=0;
	SPtrList<CSpSclParserEx::stuSCL_IED_AccessPoint_Server_LDevice> lds;
	pSclParser->RetrieveIedLd(pIed,lds);
	stuSclVtIedLd *pVtLd;
	stuSclVtIedDataset *pVtDataset;
	stuSclVtIedDatasetFcda *pVtFcda;
	//stuSclVtIedSmvIn *pVtSmvIn;
	stuSclVtIedSmvOut *pVtSmvOut;
	//stuSclVtIedSmvInChannel *pVtSmvInChn;
	//stuSclVtIedGooseIn *pVtGooseIn;
	stuSclVtIedGooseOut *pVtGooseOut;
	//stuSclVtIedGooseInChannel *pVtGooseInChn;
	char* ln_desc,*do_desc,*da_desc,*dime,*ln_type,*do_type;
	//int group_no = 0;
	
	int chn_no;
	//char last_lnInst[32],last_lnClass[32];
	//last_lnInst[0] = last_lnClass[0] = '\0';

	CSpSclParserEx::stuSCL_IED_AccessPoint_Server_LDevice_LN0 *pLN0;
	CSpSclParserEx::stuSCL_IED_AccessPoint_Server_LDevice_LN0_DataSet *pDataSet;
	CSpSclParserEx::stuSCL_IED_AccessPoint_Server_LDevice_LN0_DataSet_FCDA *pFCDA;
	//CSpSclParserEx::stuSCL_IED_AccessPoint_Server_LDevice_LN *pLN=NULL;
	//CSpSclParserEx::stuSCL_IED_AccessPoint_Server_LDevice_LN_DOI *pDoi=NULL;
	//CSpSclParserEx::stuSCL_IED_AccessPoint_Server_LDevice_LN_DOI_DAI_Val *pDaiDuVal=NULL;
	CSpSclParserEx::stuSCL_IED_AccessPoint_Server_LDevice_LN0_SampledValueControl *pSvcb;
	CSpSclParserEx::stuSCL_IED_AccessPoint_Server_LDevice_LN0_SampledValueControl_SmvOpts *pSvOpt;
	CSpSclParserEx::stuSCL_IED_AccessPoint_Server_LDevice_LN0_GSEControl *pGocb;
	CSpSclParserEx::stuSCL_IED_AccessPoint *pAp;
	//CSpSclParserEx::stuSCL_Communication_SubNetwork_ConnectedAP *pCAP;
	//CSpSclParserEx::stuSCL_Communication_SubNetwork_ConnectedAP_GSE *pCAP_GSE;
	//CSpSclParserEx::stuSCL_Communication_SubNetwork_ConnectedAP_SMV *pCAP_SMV;


	CSpSclParserEx::stuSCL_IED_AccessPoint_Server_LDevice *pLd = lds.FetchFirst(pos1);
	CSpSclParserEx::stuSCL_IED_AccessPoint_Server_LDevice *pThisLd;
	while(pLd)
	{
		//load ld
		pVtLd = new stuSclVtIedLd();
		pVtLd->inst = (char*)pLd->inst;
		pVtLd->desc = SString::safeChar(pLd->desc);
		pVtIed->m_Ld.append(pVtLd);

		//load dataset
		pLN0 = pLd->m_LN0[0];
		if(pLN0 == NULL)
		{
			LOGWARN("IED(%s)LD(%s:%s)缺少LN0节点!",pIed->name,pLd->inst,pLd->desc);
			pLd = lds.FetchNext(pos1);
			continue;
		}
		else if(pLd->m_LN0.count() > 1)
		{
			LOGWARN("IED(%s)LD(%s:%s)LN0节点不唯一!共含有%d个LN0节点!将忽略",pIed->name,pLd->inst,pLd->desc,pLd->m_LN0.count());
		}
		pDataSet = pLN0->m_DataSet.FetchFirst(pos2);
		while(pDataSet)
		{
			pVtDataset = new stuSclVtIedDataset();
			pVtIed->m_Dataset.append(pVtDataset);

			pVtDataset->ds_no = pVtIed->m_Dataset.count();
			pVtDataset->ld_name = (char*)pLd->inst;
			pVtDataset->name = (char*)pDataSet->name;
			pVtDataset->desc = SString::safeChar(pDataSet->desc);
			pVtDataset->iedname = SString::safeChar(pVtIed->name);
			//load fcda
			chn_no = 0;
			pFCDA = pDataSet->m_FCDA.FetchFirst(pos3);
			while(pFCDA)
			{
				ln_desc=do_desc=da_desc=dime = (char*)g_vt_string_null;
				pThisLd = pLd;
				ln_type = do_type = (char*)g_vt_string_null;
				if(!pSclParser->GetFcdaNodeInfo(pFCDA,pIed,pThisLd,ln_desc,do_desc,da_desc,dime,ln_type,do_type))
				{
					LOGWARN("数据集FCDA无效(IED:%s,ld:%s,lnClass:%s,lnInst:%s)",
						pIed->name,pLd->inst,pFCDA->lnClass,pFCDA->lnInst);
				}
				
				pVtFcda = new stuSclVtIedDatasetFcda();
				pVtDataset->m_Fcda.append(pVtFcda);
				pVtFcda->chn_no = ++chn_no;
				pVtFcda->ln_desc=SString::safeChar(ln_desc);
				pVtFcda->doi_desc = SString::safeChar(do_desc);
				pVtFcda->dai_desc = SString::safeChar(da_desc);
				if(strstr(pVtFcda->dai_desc,"额定延") != NULL)
				{
					pVtFcda->chn_type = SCL_VT_MU_DELAY;
					pVtFcda->primary_rated = 1;
					pVtFcda->secondary_rated = 1;
				}
				else if(strstr(pVtFcda->dai_desc,"电流") != NULL)
				{
					pVtFcda->chn_type = SCL_VT_MU_CHN_AI;
					pVtFcda->primary_rated = m_fPriaryCurrent;
					pVtFcda->secondary_rated = m_fSecondaryCurrent;
				}
				else if(strstr(pVtFcda->dai_desc,"电压") != NULL)
				{
					pVtFcda->chn_type = SCL_VT_MU_CHN_AU;
					pVtFcda->primary_rated = m_fPrimaryVoltage;
					pVtFcda->secondary_rated = m_fSecondaryVoltage;
				}
				else
				{
					pVtFcda->chn_type = SCL_VT_MU_CHN_D;
					pVtFcda->primary_rated = 1;
					pVtFcda->secondary_rated = 1;
				}
				pVtFcda->ln_type = ln_type;
				pVtFcda->do_type = do_type;
				pVtFcda->dime = dime;
				pVtFcda->bType = SString::safeChar(pSclParser->GetDaValType(do_type,pFCDA->doName,SString::safeChar(pFCDA->daName)));
				////0:未知  1:字符串  2：整数  3：浮点数  4：bitstring  5:utctime  6:bool   7:品质  8:Struct ...
				if(SString::equals(pVtFcda->bType,"VisString64") || SString::equals(pVtFcda->bType,"VisString255") || SString::equals(pVtFcda->bType,"Unicode255"))
					pVtFcda->vtype = 1;
				else if(SString::equals(pVtFcda->bType,"INT32") || SString::equals(pVtFcda->bType,"INT32U") || SString::equals(pVtFcda->bType,"Enum"))
					pVtFcda->vtype = 2;
				else if(SString::equals(pVtFcda->bType,"FLOAT32"))
					pVtFcda->vtype = 3;
				else if(SString::equals(pVtFcda->bType,"Dbpos") || SString::equals(pVtFcda->bType,"BitString"))
					pVtFcda->vtype = 4;
				else if(SString::equals(pVtFcda->bType,"Timestamp"))
					pVtFcda->vtype = 5;
				else if(SString::equals(pVtFcda->bType,"BOOLEAN") )
					pVtFcda->vtype = 6;
				else if(SString::equals(pVtFcda->bType,"Quality"))
					pVtFcda->vtype = 7;
				else if(SString::equals(pVtFcda->bType,"Struct"))
					pVtFcda->vtype = 8;
				else
					pVtFcda->vtype = 0;
				pVtFcda->path.sprintf("%s/%s%s%s$%s$%s%s%s",SString::safeChar(pThisLd->inst),SString::safeChar(pFCDA->prefix),
					SString::safeChar(pFCDA->lnClass),SString::safeChar(pFCDA->lnInst),SString::safeChar(pFCDA->fc),SString::safeChar(pFCDA->doName),
					(pFCDA->daName==NULL || pFCDA->daName[0] == '\0')?"":"$",SString::safeChar(pFCDA->daName));
				
				pFCDA = pDataSet->m_FCDA.FetchNext(pos3);
			}
			pDataSet = pLN0->m_DataSet.FetchNext(pos2);
		}

		//load smvout
		pSvcb = pLN0->m_SampledValueControl.FetchFirst(pos2);
		while(pSvcb)
		{
			pVtSmvOut = new stuSclVtIedSmvOut();
			pVtSmvOut->ied_name = pVtIed->name;
			pVtSmvOut->ds_name = pSvcb->datSet;
			pVtSmvOut->smv_no = pVtIed->m_SmvOut.count()+1;
			pVtSmvOut->name = (char*)pSvcb->name;
			pVtSmvOut->datSet = (char*)pSvcb->datSet;
			pVtSmvOut->confRev=pSvcb->confRev==NULL?0:atoi(pSvcb->confRev);
			pVtSmvOut->nofASDU=pSvcb->nofASDU==NULL?0:atoi(pSvcb->nofASDU);
			pVtSmvOut->smpRate=pSvcb->smpRate==NULL?0:atoi(pSvcb->smpRate);
			pVtSmvOut->smvID = (char*)pSvcb->smvID;
			pVtSmvOut->multicast = SString::equals(pSvcb->multicast,"true");
			pVtSmvOut->desc = (char*)pSvcb->desc;
			pAp = pSclParser->SearchIedAccessPointByLd(pIed,pLd);
			//pCAP_SMV = NULL;
			if(pAp != NULL)
			{
				char* appid=NULL;
				char* vlan=NULL,*vlan_p=NULL;
				pVtSmvOut->mac = (char*)&g_vt_string_null;
				pSclParser->GetCAPSMV_Param((char*)pIed->name,(char*)pAp->name,(char*)pVtSmvOut->name,(char*)pLd->inst,(char* &)pVtSmvOut->mac,appid,vlan_p,vlan);
				pVtSmvOut->appid = SString::toFormat("0x%s",SString::safeChar(appid)).toInt();
				pVtSmvOut->vlan_priority = vlan_p==NULL?0:atoi(vlan_p);
				pVtSmvOut->vlan =  vlan==NULL?0:atoi(vlan);


// 				pCAP = pSclParser->SearchConnectAP((char*)pIed->name,(char*)pAp->name);
// 				if(pCAP != NULL)
// 				{
// 					pCAP_SMV = pSclParser->SearchCAPSMV(pCAP,(char*)pVtSmvOut->name,(char*)pLd->inst);
// 				}
			}
			else
			//if(pCAP_SMV == NULL)
			{
				pVtSmvOut->appid = 0;
				pVtSmvOut->mac = (char*)g_vt_string_null;
				pVtSmvOut->vlan = 0;
				pVtSmvOut->vlan_priority = 0;
			}
// 			else
// 			{
// 				char* appid;
// 				char* vlan,*vlan_p;
// 				pSclParser->GetCAPSMV_Param(pCAP_SMV,(char*)pVtSmvOut->mac,appid,vlan_p,vlan);
// 				pVtSmvOut->appid = SString::toFormat("0x%s",appid).toInt();
// 				pVtSmvOut->vlan_priority = atoi(vlan_p);
// 				pVtSmvOut->vlan = atoi(vlan);
// 			}
			pSvOpt = pSvcb->m_SmvOpts[0];
			if(pSvOpt != NULL)
			{
				pVtSmvOut->refreshTime = SString::equals(pSvOpt->refreshTime,"true");
				pVtSmvOut->sampleRate = SString::equals(pSvOpt->sampleRate,"true");
				pVtSmvOut->sampleSynchronized = SString::equals(pSvOpt->sampleSynchronized,"true");
				pVtSmvOut->security = SString::equals(pSvOpt->security,"true");
				pVtSmvOut->dataRef = SString::equals(pSvOpt->dataRef,"true");
			}
			else
			{
				pVtSmvOut->refreshTime = true;
				pVtSmvOut->sampleRate = true;
				pVtSmvOut->sampleSynchronized = true;
				pVtSmvOut->security = true;
				pVtSmvOut->dataRef = true;
			}
			pVtSmvOut->m_pDataset = pVtIed->SearchDatasetByName((char*)pLd->inst,pVtSmvOut->ds_name.data());
			if(pVtSmvOut->m_pDataset == NULL)
			{
				LOGWARN("无效的SMV发布数据集:%s",pVtSmvOut->ds_name.data());
				delete pVtSmvOut;
			}
			else
			{
				pVtIed->m_SmvOut.append(pVtSmvOut);
			}
			pSvcb = pLN0->m_SampledValueControl.FetchNext(pos2);
		}

		//load gooseout
		pGocb = pLN0->m_GSEControl.FetchFirst(pos2);
		while(pGocb)
		{
			pVtGooseOut = new stuSclVtIedGooseOut();
			pVtGooseOut->ied_name = pVtIed->name;
			pVtGooseOut->ds_name = pGocb->datSet;
			pVtGooseOut->gse_no = pVtIed->m_GooseOut.count()+1;
			SString gocbRef = pGocb->appID;
			int p1;
			if((p1 = gocbRef.find("$GO$")) > 0)
				gocbRef = gocbRef.left(p1+4)+pGocb->name;
			pVtGooseOut->gocb = (char*)pSclParser->m_ConstString.CopyToConstString(gocbRef.data());// (char*)pGocb->appID;
			gocbRef = pGocb->appID;
			gocbRef.replace("$GO$","$");
			pVtGooseOut->datSet = (char*)pSclParser->m_ConstString.CopyToConstString(gocbRef.data());// (char*)pGocb->datSet;
			pVtGooseOut->confRev = pGocb->confRev==NULL?0:atoi(pGocb->confRev);
			pVtGooseOut->name = (char*)pGocb->name;
			pAp = pSclParser->SearchIedAccessPointByLd(pIed,pLd);
			//pCAP_GSE = NULL;
			if(pAp != NULL)
			{
				char* appid=NULL,*mintime=NULL,*maxtime=NULL;
				char* vlan=NULL,*vlan_p=NULL;
				pVtGooseOut->mac = (char*)g_vt_string_null;
				pSclParser->GetCAPGSE_Param((char*)pIed->name,(char*)pAp->name,(char*)pVtGooseOut->name,(char*)pLd->inst,(char* &)pVtGooseOut->mac,appid,vlan_p,vlan,mintime,maxtime);
				pVtGooseOut->appid = SString::toFormat("0x%s",SString::safeChar(appid)).toInt();
				pVtGooseOut->vlan_priority = vlan_p==NULL?0:atoi(vlan_p);
				pVtGooseOut->vlan = vlan==NULL?0:atoi(vlan);
				pVtGooseOut->min_time = mintime==NULL?0:atoi(mintime);
				pVtGooseOut->max_time = maxtime==NULL?0:atoi(maxtime);


// 				pCAP = pSclParser->SearchConnectAP((char*)pIed->name,(char*)pAp->name);
// 				if(pCAP != NULL)
// 				{
// 					pCAP_GSE = pSclParser->SearchCAPGSE(pCAP,(char*)pVtGooseOut->name,(char*)pLd->inst);
// 				}
			}
			else
			//if(pCAP_GSE == NULL)
			{
				pVtGooseOut->appid = 0;
				pVtGooseOut->mac = (char*)g_vt_string_null;
				pVtGooseOut->vlan = 0;
				pVtGooseOut->vlan_priority = 0;
				pVtGooseOut->min_time = 2;
				pVtGooseOut->max_time = 5000;
			}
// 			else
// 			{
// 				char* appid,*mintime,*maxtime;
// 				char* vlan,*vlan_p;
// 				pSclParser->GetCAPGSE_Param(pCAP_GSE,(char*)pVtGooseOut->mac,appid,vlan_p,vlan,mintime,maxtime);
// 				pVtGooseOut->appid = SString::toFormat("0x%s",appid).toInt();
// 				pVtGooseOut->vlan_priority = atoi(vlan_p);
// 				pVtGooseOut->vlan = atoi(vlan);
// 				pVtGooseOut->min_time = atoi(mintime);
// 				pVtGooseOut->max_time = atoi(maxtime);
// 			}
			pVtGooseOut->m_pDataset = pVtIed->SearchDatasetByName((char*)pLd->inst,(char*)pVtGooseOut->ds_name.data());
			if(pVtGooseOut->m_pDataset == NULL)
			{
				LOGWARN("无效的GOOSE发布数据集:%s",(char*)pVtGooseOut->ds_name.data());
				delete pVtGooseOut;
			}
			else
			{
				pVtIed->m_GooseOut.append(pVtGooseOut);
			}

			pGocb = pLN0->m_GSEControl.FetchNext(pos2);
		}


// 		//load svmin
// 		group_no = 0;
// 
// 		//load goosein
// 		group_no = 0;

		//end of LDevice
		pLd = lds.FetchNext(pos1);
	}
	
	return true;
}


bool CSclVt::LoadIedInputsBySclParser(CSpSclParserEx *pSclParser,stuSclVtIed *pVtIed,CSpSclParserEx::stuSCL_IED *pIed)
{
	unsigned long pos1=0,pos2=0,pos3=0;
	SPtrList<CSpSclParserEx::stuSCL_IED_AccessPoint_Server_LDevice> lds;
	pSclParser->RetrieveIedLd(pIed,lds);

	stuSclVtIed *pExtVtIed;
	stuSclVtIedDataset *pExtVtDataset;
	stuSclVtIedSmvOut *pExtVtSmvOut;
	stuSclVtIedGooseOut *pExtVtGooseOut;
	stuSclVtIedSmvIn *pVtSmvIn;
	stuSclVtIedSmvInChannel *pVtSmvInChn;
	stuSclVtIedGooseIn *pVtGooseIn;
	stuSclVtIedGooseInChannel *pVtGooseInChn;
	stuSclVtIedUnknownInput *pUnknownInput;

	//int group_no = 0;

	//int chn_no;
	//char last_lnInst[32],last_lnClass[32];
	//last_lnInst[0] = last_lnClass[0] = '\0';

	CSpSclParserEx::stuSCL_IED_AccessPoint_Server_LDevice_LN0 *pLN0;
	CSpSclParserEx::stuSCL_IED_AccessPoint_Server_LDevice_LN0_Inputs *pInputs;
	CSpSclParserEx::stuSCL_IED_AccessPoint_Server_LDevice_LN0_Inputs_ExtRef *pExtRef;

	CSpSclParserEx::stuSCL_IED *pExtIed;
	CSpSclParserEx::stuSCL_IED_AccessPoint_Server_LDevice *pExtLd;
	CSpSclParserEx::stuSCL_IED_AccessPoint_Server_LDevice_LN0_DataSet *pExtDataSet;
	CSpSclParserEx::stuSCL_IED_AccessPoint_Server_LDevice_LN0_SampledValueControl *pExtSvcb;
	CSpSclParserEx::stuSCL_IED_AccessPoint_Server_LDevice_LN0_GSEControl *pExtGocb;
	CSpSclParserEx::stuSCL_IED_AccessPoint_Server_LDevice_LN0_DataSet_FCDA *pExtFcda;
	CSpSclParserEx::stuSCL_IED_AccessPoint_Server_LDevice_LN_DOI *pDoi;

	CSpSclParserEx::stuSCL_IED_AccessPoint_Server_LDevice *pLd = lds.FetchFirst(pos1);
	while(pLd)
	{
		pLN0 = pLd->m_LN0[0];
		if(pLN0 == NULL)
		{
			pLd = lds.FetchNext(pos1);
			continue;
		}
		pInputs = pLN0->m_Inputs.FetchFirst(pos2);
		while(pInputs)
		{
			pExtRef = pInputs->m_ExtRef.FetchFirst(pos3);
			while(pExtRef)
			{
				pExtSvcb = NULL;
				pExtGocb = NULL;
				pExtFcda = pSclParser->SearchFCDAByExtref(pExtRef,pExtIed,pExtLd,pExtDataSet,pExtSvcb,pExtGocb);
				again:
				if(pExtFcda == NULL)
				{
					LOGWARN("无效的虚端子连接:(ied:%s, prefix:%s, doName:%s, lnInst:%s, lnClass:%s, daName:%s, intAddr:%s,ldInst:%s",
						pExtRef->iedName,pExtRef->prefix,pExtRef->doName,pExtRef->lnInst,pExtRef->lnClass,
						pExtRef->doName,pExtRef->intAddr,pExtRef->ldInst);
					pUnknownInput = new stuSclVtIedUnknownInput();
					pUnknownInput->intAddr = pExtRef->intAddr;
					pUnknownInput->iedName = pExtRef->iedName;
					pUnknownInput->ldInst = pExtRef->ldInst;
					pUnknownInput->lnClass = pExtRef->lnClass;
					pUnknownInput->lnInst = pExtRef->lnInst;
					pUnknownInput->doName = pExtRef->doName;
					pUnknownInput->daName = pExtRef->daName;
					pUnknownInput->prefix = pExtRef->prefix;
					pVtIed->m_UnknownInput.append(pUnknownInput);

					pExtRef = pInputs->m_ExtRef.FetchNext(pos3);
					continue;
				}
				pExtVtIed = SearchIedByName((char*)pExtIed->name);
				if(pExtVtIed == NULL)
				{
					pExtFcda = NULL;
					goto again;
				}
				pExtVtDataset = pExtVtIed->SearchDatasetByName((char*)pExtRef->ldInst,(char*)pExtDataSet->name);
				if(pExtVtDataset == NULL)
				{
					pExtFcda = NULL;
					goto again;
				}

				if(pExtSvcb != NULL)
				{
					pExtVtSmvOut = pExtVtIed->SearchSmvOutByDatasetName((char*)pExtVtDataset->name);
					if(pExtVtSmvOut == NULL)
					{
						pExtFcda = NULL;
						goto again;
					}
					pVtSmvIn = pVtIed->SearchSmvInByExtNo(pExtVtIed->name,pExtVtSmvOut->smv_no);
					if(pVtSmvIn == NULL)
					{
						pVtSmvIn = new stuSclVtIedSmvIn;
						pVtIed->m_SmvIn.append(pVtSmvIn);
						pVtSmvIn->int_smv_no = pVtIed->m_SmvIn.count();
						pVtSmvIn->ext_iedname = pExtVtIed->name;
						pVtSmvIn->ext_smv_no = pExtVtSmvOut->smv_no;
						pVtSmvIn->m_pIed = pExtVtIed;
						pVtSmvIn->m_pSmvOut = pExtVtSmvOut;						
					}
					pVtSmvInChn = new stuSclVtIedSmvInChannel;
					pVtSmvIn->m_Channel.append(pVtSmvInChn);
					pVtSmvInChn->int_chn_no = pVtSmvIn->m_Channel.count();
					pDoi = pSclParser->SearchDoiByPath(pIed/*pExtIed*/,pExtRef->intAddr);
					if(pDoi == NULL)
						pVtSmvInChn->int_chn_desc=(char*)g_vt_string_null;
					else
						pVtSmvInChn->int_chn_desc=pDoi->desc;
					pVtSmvInChn->int_path = pExtRef->intAddr;
					pVtSmvInChn->ext_chn_no = pExtDataSet->m_FCDA.position(pExtFcda)+1;
					pVtSmvInChn->m_pFcda = pExtVtDataset->m_Fcda[pVtSmvInChn->ext_chn_no-1];
				}
				else if(pExtGocb != NULL)
				{
					pExtVtGooseOut = pExtVtIed->SearchGooseOutByDatasetName((char*)pExtVtDataset->name);
					if(pExtVtGooseOut == NULL)
					{
						pExtFcda = NULL;
						goto again;
					}
					pVtGooseIn = pVtIed->SearchGooseInByExtNo(pExtVtIed->name,pExtVtGooseOut->gse_no);
					if(pVtGooseIn == NULL)
					{
						pVtGooseIn = new stuSclVtIedGooseIn;
						pVtIed->m_GooseIn.append(pVtGooseIn);
						pVtGooseIn->int_gse_no = pVtIed->m_GooseIn.count();
						pVtGooseIn->ext_iedname = pExtVtIed->name;
						pVtGooseIn->ext_ldname = pExtFcda->ldInst;
						pVtGooseIn->ext_gse_no = pExtVtGooseOut->gse_no;
						pVtGooseIn->m_pIed = pExtVtIed;
						pVtGooseIn->m_pGooseOut = pExtVtGooseOut;						
					}
					pVtGooseInChn = new stuSclVtIedGooseInChannel;
					pVtGooseIn->m_Channel.append(pVtGooseInChn);
					pVtGooseInChn->int_chn_no = pVtGooseIn->m_Channel.count();
					pDoi = pSclParser->SearchDoiByPath(pIed/*pExtIed*/,pExtRef->intAddr);
					if(pDoi == NULL)
						pVtGooseInChn->int_chn_desc=(char*)g_vt_string_null;
					else
						pVtGooseInChn->int_chn_desc=pDoi->desc;
					pVtGooseInChn->int_path = pExtRef->intAddr;
					pVtGooseInChn->ext_chn_no = pExtDataSet->m_FCDA.position(pExtFcda)+1;
					pVtGooseInChn->m_pFcda = pExtVtDataset->m_Fcda[pVtGooseInChn->ext_chn_no-1];
				}
				pExtRef = pInputs->m_ExtRef.FetchNext(pos3);
			}
			pInputs = pLN0->m_Inputs.FetchNext(pos2);
		}
		

		//end of LDevice
		pLd = lds.FetchNext(pos1);
	}

	return true;
}

stuSclVtIed* CSclVt::SearchIedByName(char *ied_name)
{
	unsigned long pos=0;
	register stuSclVtIed *p = m_Ied.FetchFirst(pos);
	while(p)
	{
		if(SString::equals(ied_name,p->name))
			return p;
		p = m_Ied.FetchNext(pos);
	}
	return NULL;
}

bool CSclVt::LoadByBaseConfig(SBaseConfig *pParentNode)
{
	unsigned long pos1 = 0,pos2 = 0,pos3 = 0;

	SBaseConfig *pIedNode = pParentNode->GetChildPtr()->FetchFirst(pos1);
	while(pIedNode)
	{
		if(pIedNode->GetNodeName() == "sub_network")
		{
			stuSclVtSubNetwork *pVtSubNetwork = new stuSclVtSubNetwork();
			pVtSubNetwork->name = m_ConstString.CopyToConstString(pIedNode->GetAttribute("name").data());
			pVtSubNetwork->desc = m_ConstString.CopyToConstString(pIedNode->GetAttribute("desc").data());
			pVtSubNetwork->type = m_ConstString.CopyToConstString(pIedNode->GetAttribute("type").data());
			m_SubNetwork.append(pVtSubNetwork);
			SBaseConfig *pAP = pIedNode->GetChildPtr()->FetchFirst(pos2);
			while(pAP)
			{
				if(pAP->GetNodeName() != "connect_ap")
				{
					pAP = pIedNode->GetChildPtr()->FetchNext(pos2);
					continue;
				}
				stuSclVtSubNetworkConnectAP *pVtAP = new stuSclVtSubNetworkConnectAP();
				pVtAP->ied_name = m_ConstString.CopyToConstString(pAP->GetAttribute("ied_name").data());
				pVtAP->ap_name = m_ConstString.CopyToConstString(pAP->GetAttribute("ap_name").data());
				pVtAP->desc = m_ConstString.CopyToConstString(pAP->GetAttribute("desc").data());
				pVtSubNetwork->m_ConnectAP.append(pVtAP);

				SBaseConfig *pAddress = pAP->GetChildPtr()->FetchFirst(pos3);
				while(pAddress)
				{
					if(pAddress->GetNodeName() == "address")
					{
						stuSclVtSubNetworkConnectAPAddress *p = new stuSclVtSubNetworkConnectAPAddress();
						p->ip = m_ConstString.CopyToConstString(pAddress->GetAttribute("ip").data());
						p->ip_subnet = m_ConstString.CopyToConstString(pAddress->GetAttribute("ip_subnet").data());
						p->ip_gateway = m_ConstString.CopyToConstString(pAddress->GetAttribute("ip_gateway").data());
						pVtAP->m_Address.append(p);
					}
					else if(pAddress->GetNodeName() == "smv")
					{
						stuSclVtSubNetworkConnectAPSMV *p = new stuSclVtSubNetworkConnectAPSMV();
						p->cb_name = m_ConstString.CopyToConstString(pAddress->GetAttribute("cb_name").data());
						p->ld_inst = m_ConstString.CopyToConstString(pAddress->GetAttribute("ld_inst").data());
						p->mac_address = m_ConstString.CopyToConstString(pAddress->GetAttribute("mac_address").data());
						p->appid = m_ConstString.CopyToConstString(pAddress->GetAttribute("appid").data());
						p->vlan_priority = m_ConstString.CopyToConstString(pAddress->GetAttribute("vlan_priority").data());
						p->vlan_id = m_ConstString.CopyToConstString(pAddress->GetAttribute("vlan_id").data());
						pVtAP->m_SMV.append(p);
					}
					else if(pAddress->GetNodeName() == "gse")
					{
						stuSclVtSubNetworkConnectAPGSE *p = new stuSclVtSubNetworkConnectAPGSE();
						p->cb_name = m_ConstString.CopyToConstString(pAddress->GetAttribute("cb_name").data());
						p->ld_inst = m_ConstString.CopyToConstString(pAddress->GetAttribute("ld_inst").data());
						p->mac_address = m_ConstString.CopyToConstString(pAddress->GetAttribute("mac_address").data());
						p->appid = m_ConstString.CopyToConstString(pAddress->GetAttribute("appid").data());
						p->vlan_priority = m_ConstString.CopyToConstString(pAddress->GetAttribute("vlan_priority").data());
						p->vlan_id = m_ConstString.CopyToConstString(pAddress->GetAttribute("vlan_id").data());
						pVtAP->m_GSE.append(p);
					}
					pAddress = pAP->GetChildPtr()->FetchNext(pos3);
				}

				pAP = pIedNode->GetChildPtr()->FetchNext(pos2);
			}

			pIedNode = pParentNode->GetChildPtr()->FetchNext(pos1);
			continue;
		}
		else if(pIedNode->GetNodeName() != "ied")
		{
			pIedNode = pParentNode->GetChildPtr()->FetchNext(pos1);
			continue;
		}
		//<ied name="PT2201A" desc="PST1200U：1#变压器保护A套" manufacturer="SAC" type="PST1200U" configVersion="1.30GIR.000" >
		stuSclVtIed *pIed = new stuSclVtIed();
		m_Ied.append(pIed);
		pIed->name = (char*)m_ConstString.CopyToConstString(pIedNode->GetAttribute("name").data());
		pIed->desc = (char*)m_ConstString.CopyToConstString(pIedNode->GetAttribute("desc").data());
		pIed->manufacturer = (char*)m_ConstString.CopyToConstString(pIedNode->GetAttribute("manufacturer").data());
		pIed->type = (char*)m_ConstString.CopyToConstString(pIedNode->GetAttribute("type").data());
		pIed->configVersion = (char*)m_ConstString.CopyToConstString(pIedNode->GetAttribute("configVersion").data());

		
		SBaseConfig *pGroupNode = pIedNode->GetChildPtr()->FetchFirst(pos2);
		while(pGroupNode)
		{
			if(pGroupNode->GetNodeName() == "ld")
			{
				//<ld inst="PROT" desc="保护LD" />
				stuSclVtIedLd *pLd = new stuSclVtIedLd();
				pIed->m_Ld.append(pLd);
				pLd->inst = (char*)m_ConstString.CopyToConstString(pGroupNode->GetAttribute("inst").data());
				pLd->desc = (char*)m_ConstString.CopyToConstString(pGroupNode->GetAttribute("desc").data());
			}
			else if(pGroupNode->GetNodeName() == "dataset")
			{
				//<dataset ds_no="1" name="dsAlarm" desc="告警">
				stuSclVtIedDataset *pDataset = new stuSclVtIedDataset();
				pIed->m_Dataset.append(pDataset);
				pDataset->ds_no = pGroupNode->GetAttributeI("ds_no");
				pDataset->ld_name = (char*)m_ConstString.CopyToConstString(pGroupNode->GetAttribute("ld_name").data());
				pDataset->name = (char*)m_ConstString.CopyToConstString(pGroupNode->GetAttribute("name").data());
				pDataset->desc = (char*)m_ConstString.CopyToConstString(pGroupNode->GetAttribute("desc").data());
				pDataset->iedname = pIed->name;
				//<fcda chn_no="1" ln_type="SAC_IED_LLN0_PROT" do_type="CN_SPS_EX" ln_desc="" doi_desc="告警总信号" dai_desc="告警总信号" vtype="1" dime="" path="PROT/LLN0$ST$AlmSig" value="" />
				SBaseConfig *pFcdaNode = pGroupNode->GetChildPtr()->FetchFirst(pos3);
				while(pFcdaNode)
				{
					stuSclVtIedDatasetFcda *pFcda = new stuSclVtIedDatasetFcda();
					pDataset->m_Fcda.append(pFcda);
					pFcda->chn_no = pFcdaNode->GetAttributeI("chn_no");
					pFcda->chn_type = pFcdaNode->GetAttributeI("chn_type");
					pFcda->ln_type = (char*)m_ConstString.CopyToConstString(pFcdaNode->GetAttribute("ln_type").data());
					pFcda->do_type = (char*)m_ConstString.CopyToConstString(pFcdaNode->GetAttribute("do_type").data());
					pFcda->ln_desc = (char*)m_ConstString.CopyToConstString(pFcdaNode->GetAttribute("ln_desc").data());
					pFcda->dai_desc = (char*)m_ConstString.CopyToConstString(pFcdaNode->GetAttribute("dai_desc").data());
					pFcda->doi_desc = (char*)m_ConstString.CopyToConstString(pFcdaNode->GetAttribute("doi_desc").data());
					pFcda->bType = (char*)m_ConstString.CopyToConstString(pFcdaNode->GetAttribute("bType").data());
					pFcda->vtype = pFcdaNode->GetAttributeI("vtype");
					pFcda->dime = (char*)m_ConstString.CopyToConstString(pFcdaNode->GetAttribute("dime").data());
					pFcda->path = pFcdaNode->GetAttribute("path");
					pFcda->value = pFcdaNode->GetAttribute("value");
					pFcda->primary_rated = pFcdaNode->GetAttributeF("primaryRated");
					pFcda->secondary_rated = pFcdaNode->GetAttributeF("secondaryRated");

					pFcdaNode = pGroupNode->GetChildPtr()->FetchNext(pos3);
				}
			}
			else if(pGroupNode->GetNodeName() == "smv-out")
			{
				//<smv-out smv_no="1" name="MSVCB01" datSet="dsSV1" confRev="1" nofASDU="1" smpRate="80" smvID="MT2201AMU/LLN0$MS$MSVCB01" multicast="true" desc="" 
				//appid="0xFA1" mac="01-0C-CD-04-00-01" vlan="0" vlan_priority="4" refreshTime="false" sampleRate="true" sampleSynchronized="true" security="false" dataRef="false" />
				stuSclVtIedSmvOut *pSmvOut = new stuSclVtIedSmvOut();
				pIed->m_SmvOut.append(pSmvOut);
				pSmvOut->ied_name = pIed->name;
				pSmvOut->ds_name = pGroupNode->GetAttribute("ds_name");
				pSmvOut->smv_no = pGroupNode->GetAttributeI("smv_no");
				pSmvOut->name = (char*)m_ConstString.CopyToConstString(pGroupNode->GetAttribute("name").data());
				pSmvOut->datSet = (char*)m_ConstString.CopyToConstString(pGroupNode->GetAttribute("datSet").data());
				pSmvOut->confRev = pGroupNode->GetAttributeI("confRev");
				pSmvOut->nofASDU = pGroupNode->GetAttributeI("nofASDU");
				pSmvOut->smpRate = pGroupNode->GetAttributeI("smpRate");
				pSmvOut->smvID = (char*)m_ConstString.CopyToConstString(pGroupNode->GetAttribute("smvID").data());
				pSmvOut->desc = (char*)m_ConstString.CopyToConstString(pGroupNode->GetAttribute("desc").data());
				pSmvOut->mac = (char*)m_ConstString.CopyToConstString(pGroupNode->GetAttribute("mac").data());
				pSmvOut->appid = pGroupNode->GetAttribute("appid").toInt();
				pSmvOut->vlan = pGroupNode->GetAttributeI("vlan");
				pSmvOut->vlan_priority = pGroupNode->GetAttributeI("vlan_priority");
				pSmvOut->multicast = SString::equals(pGroupNode->GetAttribute("multicast").data(),"true");
				pSmvOut->refreshTime = SString::equals(pGroupNode->GetAttribute("refreshTime").data(),"true");
				pSmvOut->sampleRate = SString::equals(pGroupNode->GetAttribute("sampleRate").data(),"true");
				pSmvOut->sampleSynchronized = SString::equals(pGroupNode->GetAttribute("sampleSynchronized").data(),"true");
				pSmvOut->security = SString::equals(pGroupNode->GetAttribute("security").data(),"true");
				pSmvOut->dataRef = SString::equals(pGroupNode->GetAttribute("dataRef").data(),"true");
				pSmvOut->m_pDataset = pIed->SearchDatasetByName(NULL,pSmvOut->ds_name.data());
			}
			else if(pGroupNode->GetNodeName() == "goose-out")
			{
				//<goose-out gse_no="1" gocb="MT2201ARPIT/LLN0$GO$gocb1" datSet="dsGOOSE1" confRev="1" name="gocb1" appid="0x489" 
				//mac="01-0C-CD-01-01-61" vlan="0" vlan_priority="6" min_time="2" max_time="5000" />
				stuSclVtIedGooseOut *pGooseOut = new stuSclVtIedGooseOut();
				pIed->m_GooseOut.append(pGooseOut);
				pGooseOut->ied_name = pIed->name;
				pGooseOut->ds_name = pGroupNode->GetAttribute("ds_name");
				pGooseOut->gse_no = pGroupNode->GetAttributeI("gse_no");
				pGooseOut->gocb = (char*)m_ConstString.CopyToConstString(pGroupNode->GetAttribute("gocb").data());
				pGooseOut->datSet = (char*)m_ConstString.CopyToConstString(pGroupNode->GetAttribute("datSet").data());
				pGooseOut->name = (char*)m_ConstString.CopyToConstString(pGroupNode->GetAttribute("name").data());
				pGooseOut->mac = (char*)m_ConstString.CopyToConstString(pGroupNode->GetAttribute("mac").data());
				pGooseOut->confRev = pGroupNode->GetAttributeI("confRev");
				pGooseOut->appid = pGroupNode->GetAttribute("appid").toInt();
				pGooseOut->vlan = pGroupNode->GetAttributeI("vlan");
				pGooseOut->vlan_priority = pGroupNode->GetAttributeI("vlan_priority");
				pGooseOut->min_time = pGroupNode->GetAttributeI("min_time");
				pGooseOut->max_time = pGroupNode->GetAttributeI("max_time");
				pGooseOut->m_pDataset = pIed->SearchDatasetByName(NULL,pGooseOut->ds_name.data());
			}
			else if(pGroupNode->GetNodeName() == "smv-in")
			{
				stuSclVtIedSmvIn *pSmvIn = new stuSclVtIedSmvIn();
				pIed->m_SmvIn.append(pSmvIn);
				//<smv-in int_smv_no="1" ext_iedname="MM110A" ext_smv_no="1">
				pSmvIn->int_smv_no = pGroupNode->GetAttributeI("int_smv_no");
				pSmvIn->ext_iedname = (char*)m_ConstString.CopyToConstString(pGroupNode->GetAttribute("ext_iedname").data());
				pSmvIn->ext_smv_no = pGroupNode->GetAttributeI("ext_smv_no");

				SBaseConfig *pChnNode = pGroupNode->GetChildPtr()->FetchFirst(pos3);
				while(pChnNode)
				{
					//<channel int_chn_no="1" int_chn_desc="" int_path="MU/SVINTVTR1.Vol1.instMag.i" ext_chn_no="2" />
					stuSclVtIedSmvInChannel *pChn = new stuSclVtIedSmvInChannel();
					pSmvIn->m_Channel.append(pChn);
					pChn->int_chn_no = pChnNode->GetAttributeI("int_chn_no");
					pChn->int_chn_desc = (char*)m_ConstString.CopyToConstString(pChnNode->GetAttribute("int_chn_desc").data());
					pChn->int_path = (char*)m_ConstString.CopyToConstString(pChnNode->GetAttribute("int_path").data());
					pChn->ext_chn_no = pChnNode->GetAttributeI("ext_chn_no");
					pChnNode = pGroupNode->GetChildPtr()->FetchNext(pos3);
				}
			}
			else if(pGroupNode->GetNodeName() == "goose-in")
			{
				stuSclVtIedGooseIn *pGooseIn = new stuSclVtIedGooseIn();
				pIed->m_GooseIn.append(pGooseIn);
				//<goose-in int_gse_no="1" ext_iedname="IT1101A" ext_ldname="RPIT" ext_gse_no="1">
				pGooseIn->int_gse_no = pGroupNode->GetAttributeI("int_gse_no");
				pGooseIn->ext_iedname = (char*)m_ConstString.CopyToConstString(pGroupNode->GetAttribute("ext_iedname").data());
				pGooseIn->ext_ldname = (char*)m_ConstString.CopyToConstString(pGroupNode->GetAttribute("ext_ldname").data());
				pGooseIn->ext_gse_no = pGroupNode->GetAttributeI("ext_gse_no");

				SBaseConfig *pChnNode = pGroupNode->GetChildPtr()->FetchFirst(pos3);
				while(pChnNode)
				{
					//<channel int_chn_no="1" int_chn_desc="" int_path="RPIT/GOINGGIO1.DPCSO1.stVal" ext_chn_no="3" />
					stuSclVtIedGooseInChannel *pChn = new stuSclVtIedGooseInChannel();
					pGooseIn->m_Channel.append(pChn);
					pChn->int_chn_no = pChnNode->GetAttributeI("int_chn_no");
					pChn->int_chn_desc = (char*)m_ConstString.CopyToConstString(pChnNode->GetAttribute("int_chn_desc").data());
					pChn->int_path = (char*)m_ConstString.CopyToConstString(pChnNode->GetAttribute("int_path").data());
					pChn->ext_chn_no = pChnNode->GetAttributeI("ext_chn_no");
					pChnNode = pGroupNode->GetChildPtr()->FetchNext(pos3);
				}
			}


			pGroupNode = pIedNode->GetChildPtr()->FetchNext(pos2);
		}
		
		pIedNode = pParentNode->GetChildPtr()->FetchNext(pos1);
	}

	//处理所有链接指针
	stuSclVtIed *pIed = m_Ied.FetchFirst(pos1);
	while(pIed)
	{
		stuSclVtIedSmvIn *pSmvIn = pIed->m_SmvIn.FetchFirst(pos2);
		while(pSmvIn)
		{
			pSmvIn->m_pIed = SearchIedByName((char*)pSmvIn->ext_iedname);
			if(pSmvIn->m_pIed == NULL)
				pSmvIn->m_pSmvOut = NULL;
			else
				pSmvIn->m_pSmvOut = pSmvIn->m_pIed->SearchSmvOutByNo(pSmvIn->ext_smv_no);

			stuSclVtIedSmvInChannel *pChn = pSmvIn->m_Channel.FetchFirst(pos3);
			while(pChn)
			{
				if(pSmvIn->m_pSmvOut == NULL || pSmvIn->m_pSmvOut->m_pDataset == NULL)
					pChn->m_pFcda = NULL;
				else
					pChn->m_pFcda = pSmvIn->m_pSmvOut->m_pDataset->SearchFcdaByChnNo(pChn->ext_chn_no);
				pChn = pSmvIn->m_Channel.FetchNext(pos3);
			}

			pSmvIn = pIed->m_SmvIn.FetchNext(pos2);
		}

		stuSclVtIedGooseIn *pGooseIn = pIed->m_GooseIn.FetchFirst(pos2);
		while(pGooseIn)
		{
			pGooseIn->m_pIed = SearchIedByName((char*)pGooseIn->ext_iedname);
			if(pGooseIn->m_pIed == NULL)
				pGooseIn->m_pGooseOut = NULL;
			else
				pGooseIn->m_pGooseOut = pGooseIn->m_pIed->SearchGooseOutByNo(pGooseIn->ext_gse_no);

			stuSclVtIedGooseInChannel *pChn = pGooseIn->m_Channel.FetchFirst(pos3);
			while(pChn)
			{
				if(pGooseIn->m_pGooseOut == NULL || pGooseIn->m_pGooseOut->m_pDataset == NULL)
					pChn->m_pFcda = NULL;
				else
					pChn->m_pFcda = pGooseIn->m_pGooseOut->m_pDataset->SearchFcdaByChnNo(pChn->ext_chn_no);
				pChn = pGooseIn->m_Channel.FetchNext(pos3);
			}

			pGooseIn = pIed->m_GooseIn.FetchNext(pos2);
		}

		pIed = m_Ied.FetchNext(pos1);
	}
	
	return true;
}

stuSclVtIedLd* stuSclVtIed::SearchLdByName(char* ld_name)
{
	unsigned long pos=0;
	register stuSclVtIedLd *p = m_Ld.FetchFirst(pos);
	while(p)
	{
		if(SString::equals(ld_name,p->inst))
			return p;
		p = m_Ld.FetchNext(pos);
	}
	return NULL;
}

stuSclVtIedDataset* stuSclVtIed::SearchDatasetByName(char* ld_name,char* ds_name)
{
	unsigned long pos=0;
	register stuSclVtIedDataset *p = m_Dataset.FetchFirst(pos);
	while(p)
	{
		if((ld_name == NULL || SString::equals(ld_name,p->ld_name)) && SString::equals(ds_name,p->name))
			return p;
		p = m_Dataset.FetchNext(pos);
	}
	return NULL;
}

stuSclVtIedSmvOut* stuSclVtIed::SearchSmvOutByNo(int smv_no)
{
	unsigned long pos=0;
	register stuSclVtIedSmvOut *p = m_SmvOut.FetchFirst(pos);
	while(p)
	{
		if(smv_no == p->smv_no)
			return p;
		p = m_SmvOut.FetchNext(pos);
	}
	return NULL;
}

stuSclVtIedGooseOut* stuSclVtIed::SearchGooseOutByNo(int goose_no)
{
	unsigned long pos=0;
	register stuSclVtIedGooseOut *p = m_GooseOut.FetchFirst(pos);
	while(p)
	{
		if(goose_no == p->gse_no)
			return p;
		p = m_GooseOut.FetchNext(pos);
	}
	return NULL;
}

stuSclVtIedSmvOut* stuSclVtIed::SearchSmvOutByDatasetName(char* ds_name)
{
	unsigned long pos=0;
	register stuSclVtIedSmvOut *p = m_SmvOut.FetchFirst(pos);
	while(p)
	{
		if(SString::equals(p->ds_name.data(),ds_name))
			return p;
		p = m_SmvOut.FetchNext(pos);
	}
	return NULL;
}

stuSclVtIedGooseOut* stuSclVtIed::SearchGooseOutByDatasetName(char* ds_name)
{
	unsigned long pos=0;
	register stuSclVtIedGooseOut *p = m_GooseOut.FetchFirst(pos);
	while(p)
	{
		if(SString::equals(p->ds_name.data(),ds_name))
			return p;
		p = m_GooseOut.FetchNext(pos);
	}
	return NULL;
}

stuSclVtIedSmvOut* stuSclVtIed::SearchSmvOutByAppid(int appid)
{
	unsigned long pos=0;
	register stuSclVtIedSmvOut *p = m_SmvOut.FetchFirst(pos);
	while(p)
	{
		if(p->appid == appid)
			return p;
		p = m_SmvOut.FetchNext(pos);
	}
	return NULL;
}

stuSclVtIedGooseOut* stuSclVtIed::SearchGooseOutByAppid(int appid)
{
	unsigned long pos=0;
	register stuSclVtIedGooseOut *p = m_GooseOut.FetchFirst(pos);
	while(p)
	{
		if(p->appid == appid)
			return p;
		p = m_GooseOut.FetchNext(pos);
	}
	return NULL;
}

stuSclVtIedSmvIn* stuSclVtIed::SearchSmvInByNo(int int_smv_no)
{
	unsigned long pos=0;
	register stuSclVtIedSmvIn *p = m_SmvIn.FetchFirst(pos);
	while(p)
	{
		if(int_smv_no == p->int_smv_no)
			return p;
		p = m_SmvIn.FetchNext(pos);
	}
	return NULL;
}

stuSclVtIedGooseIn* stuSclVtIed::SearchGooseInByNo(int int_goose_no)
{
	unsigned long pos=0;
	register stuSclVtIedGooseIn *p = m_GooseIn.FetchFirst(pos);
	while(p)
	{
		if(int_goose_no == p->int_gse_no)
			return p;
		p = m_GooseIn.FetchNext(pos);
	}
	return NULL;
}

stuSclVtIedSmvIn* stuSclVtIed::SearchSmvInByExtNo(const char* ext_iedname,int ext_smv_no)
{
	unsigned long pos=0;
	register stuSclVtIedSmvIn *p = m_SmvIn.FetchFirst(pos);
	while(p)
	{
		if(SString::equals(p->ext_iedname,ext_iedname) && p->ext_smv_no == ext_smv_no)
			return p;
		p = m_SmvIn.FetchNext(pos);
	}
	return NULL;
}

stuSclVtIedGooseIn* stuSclVtIed::SearchGooseInByExtNo(const char* ext_iedname,int ext_goose_no)
{
	unsigned long pos=0;
	register stuSclVtIedGooseIn *p = m_GooseIn.FetchFirst(pos);
	while(p)
	{
		if(SString::equals(p->ext_iedname,ext_iedname) && p->ext_gse_no == ext_goose_no)
			return p;
		p = m_GooseIn.FetchNext(pos);
	}
	return NULL;
}

