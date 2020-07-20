/**
 *
 * 文 件 名 : sim_config.cpp
 * 创建日期 : 2015-12-22 13:33
 * 作    者 : 邵凯田(skt001@163.com)
 * 修改日期 : $Date: $
 * 当前版本 : $Revision: $
 * 功能描述 : SIM配置文件类
 * 修改记录 : 
 *            $Log: $
 *
 * Ver  Date        Author  Comments
 * ---  ----------  ------  -------------------------------------------
 * 001	2015-12-22	邵凯田　创建文件
 *
 **/

#include "sim_config.h"
#include "SXmlConfig.h"
CSimConfig::CSimConfig()
{
	m_SubNet.setAutoDelete(true);
	m_Scripts.setAutoDelete(true);
	m_iEditSn = 0;
}

CSimConfig::~CSimConfig()
{
	m_SubNet.clear();
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  加载配置文件
// 作    者:  邵凯田
// 创建时间:  2015-12-22 13:35
// 参数说明:  sFile
// 返 回 值:  true/false
//////////////////////////////////////////////////////////////////////////
bool CSimConfig::Load(SString sFile)
{
	SXmlConfig xml;
	m_sCfgFile = sFile;
	if(!xml.ReadConfig(sFile))
		return false;
	m_iEditSn = 0;
	m_SubNet.clear();
	//<scd file="sim.scd" />
	//m_sScdFile = xml.SearchNodeAttribute("scd","file");
	stuSimSubnetwork *pSubNet;
	SBaseConfig *pSubNetNode;
	int i,cnt;
	SBaseConfig *pIedList = xml.SearchChild("ied-list");
	if(pIedList != NULL)
	{
		m_VirtualTerminal.m_sStationName = pIedList->GetAttribute("station_name");
		m_VirtualTerminal.LoadByBaseConfig(pIedList);
	}
	m_bOpenMmsServer = xml.SearchNodeAttribute("mms-server","open")=="true";
	m_bMmsSvrSimIpAddr = xml.SearchNodeAttribute("mms-server","sim_ip")=="true";
	m_iMmsStTimes = xml.SearchNodeAttributeI("mms-server","st_times");
	m_iMmsMxTimes = xml.SearchNodeAttributeI("mms-server","mx_times");
	m_sMmsExtAttribute = xml.SearchNodeAttribute("mms-server","ext_attribute");
	cnt = xml.GetChildCount("subnetwork");
	for(i=0;i<cnt;i++)
	{
		pSubNetNode = xml.GetChildNode(i,"subnetwork");
		if(pSubNetNode == NULL)
			continue;
		pSubNet = new stuSimSubnetwork();
		//<subnetwork name="eth1" desc="网口1" ip="" >
		pSubNet->name = pSubNetNode->GetAttribute("name");
		pSubNet->desc = pSubNetNode->GetAttribute("desc");
		pSubNet->ip = pSubNetNode->GetAttribute("ip");
		pSubNet->mac = pSubNetNode->GetAttribute("mac");
		pSubNet->netmask = pSubNetNode->GetAttribute("netmask");
		pSubNet->type = pSubNetNode->GetAttribute("type");
		pSubNet->local_netcard= pSubNetNode->GetAttribute("local_netcard");
		m_SubNet.append(pSubNet);

		unsigned long pos = 0;
		SBaseConfig *pSim = pSubNetNode->GetChildPtr()->FetchFirst(pos);
		while(pSim)
		{
			if(pSim->GetNodeName() == "smv")
			{
				//<smv ied_name="" smv_no="1" />
				stuSclVtIed *pVtIed = m_VirtualTerminal.SearchIedByName(pSim->GetAttribute("ied_name").data());
				if(pVtIed == NULL)
				{
					LOGWARN("无效的IED名称[%s]!",pSim->GetAttribute("ied_name").data());
				}
				else
				{
					stuSclVtIedSmvOut *pOut = pVtIed->SearchSmvOutByNo(pSim->GetAttributeI("smv_no"));
					if(pOut == NULL)
					{
						LOGWARN("装置[%s]下发现无效的SMV引用[%d]!",pVtIed->name,pSim->GetAttributeI("smv_no"));
					}
					else
					{
						pSubNet->m_Smv.append(pOut);
					}
				}
			}
			else if(pSim->GetNodeName() == "goose")
			{
				//<goose ied_name="" gse_no="1" />
				stuSclVtIed *pVtIed = m_VirtualTerminal.SearchIedByName(pSim->GetAttribute("ied_name").data());
				if(pVtIed == NULL)
				{
					LOGWARN("无效的IED名称[%s]!",pSim->GetAttribute("ied_name").data());
				}
				else
				{
					stuSclVtIedGooseOut *pOut = pVtIed->SearchGooseOutByNo(pSim->GetAttributeI("gse_no"));
					if(pOut == NULL)
					{
						LOGWARN("装置[%s]下发现无效的GOOSE引用[%d]!",pVtIed->name,pSim->GetAttributeI("gse_no"));
					}
					else
					{
						pSubNet->m_Goose.append(pOut);
					}
				}
			}
			else if(pSim->GetNodeName() == "mms_svr")
			{
				stuSimMmsSvr *pMmsSvr = new stuSimMmsSvr();
				pMmsSvr->iedname	= pSim->GetAttribute("iedname");
				pMmsSvr->comtrade_path = pSim->GetAttribute("comtrade_path");
				pSubNet->m_MmsSvr.append(pMmsSvr);
			}
			else if(pSim->GetNodeName() == "mms_client")
			{
				stuSimMmsClient *pMmsClient = new stuSimMmsClient();
				pMmsClient->iedname	= pSim->GetAttribute("iedname");
				pMmsClient->ip		= pSim->GetAttribute("ip");
				pMmsClient->rpt_id	= pSim->GetAttributeI("rpt_id");
				pSubNet->m_MmsClient.append(pMmsClient);
			}
			else if(pSim->GetNodeName() == "ntp_svr")
			{
				stuSimNtpSvr *pNtpSvr = new stuSimNtpSvr();
				pNtpSvr->iedname	= pSim->GetAttribute("iedname");
				pSubNet->m_NtpSvr.append(pNtpSvr);
			}
			else if(pSim->GetNodeName() == "ntp_client")
			{
				stuSimNtpClient *pNtpClient = new stuSimNtpClient();
				pNtpClient->iedname	= pSim->GetAttribute("iedname");
				pNtpClient->ip		= pSim->GetAttribute("ip");
				pNtpClient->interval_sec = pSim->GetAttributeI("interval_sec");
				pSubNet->m_NtpClient.append(pNtpClient);
			}
			else if(pSim->GetNodeName() == "tcp_svr")
			{
				stuSimTcpSvr *pNew = new stuSimTcpSvr();
				pNew->port				= pSim->GetAttributeI("port");
				pNew->send_len			= pSim->GetAttributeI("send_len");
				pNew->send_interval_us	= pSim->GetAttributeI("send_interval_us");
				pSubNet->m_TcpSvr.append(pNew);
			}
			else if(pSim->GetNodeName() == "tcp_client")
			{
				stuSimTcpClient *pNew = new stuSimTcpClient();
				pNew->ip				= pSim->GetAttribute("ip");
				pNew->port				= pSim->GetAttributeI("port");
				pNew->send_len			= pSim->GetAttributeI("send_len");
				pNew->send_interval_us	= pSim->GetAttributeI("send_interval_us");
				pSubNet->m_TcpClient.append(pNew);
			}
			else if(pSim->GetNodeName() == "udp_send")
			{
				stuSimUdpSend *pNew = new stuSimUdpSend();
				pNew->ip				= pSim->GetAttribute("ip");
				pNew->port				= pSim->GetAttributeI("port");
				pNew->send_len			= pSim->GetAttributeI("send_len");
				pNew->send_interval_us	= pSim->GetAttributeI("send_interval_us");
				pSubNet->m_UdpSend.append(pNew);
			}
			else if(pSim->GetNodeName() == "icmp_send")
			{
				stuSimIcmpSend *pNew = new stuSimIcmpSend();
				pNew->ip				= pSim->GetAttribute("ip");
				pNew->send_len			= pSim->GetAttributeI("send_len");
				pNew->send_interval_us	= pSim->GetAttributeI("send_interval_us");
				pSubNet->m_IcmpSend.append(pNew);
			}
			else if(pSim->GetNodeName() == "arp_send")
			{
				stuSimArpSend *pNew = new stuSimArpSend();
				pNew->send_len			= pSim->GetAttributeI("send_len");
				pNew->send_interval_us	= pSim->GetAttributeI("send_interval_us");
				pSubNet->m_ArpSend.append(pNew);
			}
			else if(pSim->GetNodeName() == "storm")
			{
				stuSimStorm *pNew = new stuSimStorm();
				pNew->send_len			= pSim->GetAttributeI("send_len");
				pNew->send_interval_us	= pSim->GetAttributeI("send_interval_us");
				pSubNet->m_Storm.append(pNew);
			}
			pSim = pSubNetNode->GetChildPtr()->FetchNext(pos);
		}
	}
	SBaseConfig *pMmsServer = xml.SearchChild("mms_server");
	if(pMmsServer != NULL)
	{
		unsigned long pos1 =0;
		SBaseConfig *pIedNode = pMmsServer->GetChildPtr()->FetchFirst(pos1);
		while(pIedNode)
		{
			stuSclVtIed *pVtIed = m_VirtualTerminal.SearchIedByName(pIedNode->GetAttribute("ied_name").data());
			if(pVtIed == NULL)
			{
				LOGWARN("无效的IED名称[%s]!",pIedNode->GetAttribute("ied_name").data());
			}
			else
			{
				m_MmsServerIed.append(pVtIed);
			}
			
			pIedNode = pMmsServer->GetChildPtr()->FetchNext(pos1);
		}
	}

	SBaseConfig *pRunScript = xml.SearchChild("run_scripts");
	if(pRunScript != NULL)
	{
		unsigned long pos1 =0;
		SBaseConfig *pScript = pRunScript->GetChildPtr()->FetchFirst(pos1);
		while(pScript)
		{
			if(pScript->GetNodeName() == "script")
			{
				stuSimRunScript *pNew = new stuSimRunScript();
				pNew->name = pScript->GetAttribute("name");
				pNew->run_times = pScript->GetAttributeI("run_times");
				pNew->run_sepms = pScript->GetAttributeI("run_sepms");
				pNew->script = pScript->GetNodeValue();
				pNew->rt_running = false;
				pNew->rt_times = 0;
				m_Scripts.append(pNew);
			}
			pScript = pRunScript->GetChildPtr()->FetchNext(pos1);
		}
	}
	return true;
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  根据名称查找指定的子网
// 作    者:  邵凯田
// 创建时间:  2016-11-12 10:15
// 参数说明:  @sub_name表示子网名称
// 返 回 值:  stuSimSubnetwork*, NULL表示找不到
//////////////////////////////////////////////////////////////////////////
stuSimSubnetwork* CSimConfig::SearchSubnetworkByName(const char *sub_name)
{
	unsigned long pos;
	stuSimSubnetwork *p = m_SubNet.FetchFirst(pos);
	while(p)
	{
		if(p->name == sub_name)
			return p;
		p = m_SubNet.FetchNext(pos);
	}
	return NULL;
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  保存配置文件
// 作    者:  邵凯田
// 创建时间:  2015-12-22 13:52
// 参数说明:  sFile
// 返 回 值:  true/false
//////////////////////////////////////////////////////////////////////////
bool CSimConfig::Save(SString sFile)
{
	m_iEditSn++;
	if(sFile.length() == 0)
		sFile = m_sCfgFile;
	SFile f(sFile);
	if(!f.open(IO_Truncate))
		return false;
	unsigned long pos1,pos2,pos3;
	f.fprintf("<?xml version=\"1.0\" encoding=\"GBK\"?>\r\n<sim>\r\n");

	f.fprintf("\t<mms-server open=\"%s\" sim_ip=\"%s\" st_times=\"%d\" mx_times=\"%d\" ext_attribute=\"%s\" />\r\n",
		m_bOpenMmsServer?"true":"false",m_bMmsSvrSimIpAddr?"true":"false",m_iMmsStTimes,m_iMmsMxTimes,m_sMmsExtAttribute.data());

	if(m_Scripts.count()>0)
	{
		f.fprintf("\t<run_scripts>\r\n");
		stuSimRunScript *pScript = m_Scripts.FetchFirst(pos1);
		while(pScript)
		{
			f.fprintf(	"\t\t<script name=\"%s\" run_times=\"%d\" run_sepms=\"%d\">\r\n"
						"\t\t<![CDATA[",pScript->name.data(),pScript->run_times,pScript->run_sepms);
			f.writeString(pScript->script);
			f.fprintf(	"]]>\r\n"
						"\t\t</script>\r\n");
			pScript = m_Scripts.FetchNext(pos1);
		}
		f.fprintf("\t</run_scripts>\r\n");
	}

	//f.fprintf("\t<scd file=\"%s\" />\r\n",m_sScdFile.data());
	f.fprintf("\t<ied-list station_name=\"%s\">\r\n",m_VirtualTerminal.m_sStationName.data());
	stuSclVtSubNetwork *pSubNetwork = m_VirtualTerminal.m_SubNetwork.FetchFirst(pos1);
	while(pSubNetwork)
	{
		f.fprintf("\t\t<sub_network desc=\"%s\" name=\"%s\" type=\"%s\" >\r\n",
			pSubNetwork->desc,pSubNetwork->name,pSubNetwork->type);
		stuSclVtSubNetworkConnectAP *pAP = pSubNetwork->m_ConnectAP.FetchFirst(pos2);
		while(pAP)
		{
			f.fprintf("\t\t\t<connect_ap ied_name=\"%s\" ap_name=\"%s\" desc=\"%s\" >\r\n",
				pAP->ied_name,pAP->ap_name,pAP->desc);

			stuSclVtSubNetworkConnectAPAddress *pAddress = pAP->m_Address.FetchFirst(pos3);
			while(pAddress)
			{
				f.fprintf("\t\t\t\t<address ip=\"%s\" ip_subnet=\"%s\" ip_gateway=\"%s\" />\r\n",
					pAddress->ip,pAddress->ip_subnet,pAddress->ip_gateway);
				pAddress = pAP->m_Address.FetchNext(pos3);
			}

			stuSclVtSubNetworkConnectAPGSE *pGSE = pAP->m_GSE.FetchFirst(pos3);
			while(pGSE)
			{
				f.fprintf("\t\t\t\t<gse cb_name=\"%s\" ld_inst=\"%s\" mac_address=\"%s\" appid=\"%s\" vlan_priority=\"%s\" vlan_id=\"%s\" />\r\n",
					pGSE->cb_name,pGSE->ld_inst,pGSE->mac_address,pGSE->appid,pGSE->vlan_priority,pGSE->vlan_id);
				pGSE = pAP->m_GSE.FetchNext(pos3);
			}

			stuSclVtSubNetworkConnectAPSMV *pSMV = pAP->m_SMV.FetchFirst(pos3);
			while(pSMV)
			{
				f.fprintf("\t\t\t\t<smv cb_name=\"%s\" ld_inst=\"%s\" mac_address=\"%s\" appid=\"%s\" vlan_priority=\"%s\" vlan_id=\"%s\" />\r\n",
					pSMV->cb_name,pSMV->ld_inst,pSMV->mac_address,pSMV->appid,pSMV->vlan_priority,pSMV->vlan_id);
				pSMV = pAP->m_SMV.FetchNext(pos3);
			}

			f.fprintf("\t\t\t</connect_ap>\r\n");
			pAP = pSubNetwork->m_ConnectAP.FetchNext(pos2);
		}


		f.fprintf("\t\t</sub_network>\r\n");
		pSubNetwork = m_VirtualTerminal.m_SubNetwork.FetchNext(pos1);
	}


	stuSclVtIed *pIed = m_VirtualTerminal.m_Ied.FetchFirst(pos1);
	while(pIed)
	{
		f.fprintf("\t\t<ied name=\"%s\" desc=\"%s\" manufacturer=\"%s\" type=\"%s\" configVersion=\"%s\" >\r\n",
			pIed->name,pIed->desc,pIed->manufacturer,pIed->type,pIed->configVersion);
		stuSclVtIedLd *pLd = pIed->m_Ld.FetchFirst(pos2);
		while(pLd)
		{
			f.fprintf("\t\t\t<ld inst=\"%s\" desc=\"%s\" />\r\n",
				pLd->inst,SString::safeChar(pLd->desc));
			pLd = pIed->m_Ld.FetchNext(pos2);
		}

		//dataset
		stuSclVtIedDataset *pDataset = pIed->m_Dataset.FetchFirst(pos2);
		while(pDataset)
		{
			f.fprintf("\t\t\t<dataset ds_no=\"%d\" ld_name=\"%s\" name=\"%s\" desc=\"%s\">\r\n",
				pDataset->ds_no,pDataset->ld_name,pDataset->name,pDataset->desc);
			stuSclVtIedDatasetFcda *pFcda = pDataset->m_Fcda.FetchFirst(pos3);
			while(pFcda)
			{
				f.fprintf("\t\t\t\t<fcda chn_no=\"%d\" chn_type=\"%d\" ln_type=\"%s\" do_type=\"%s\" ln_desc=\"%s\" doi_desc=\"%s\" dai_desc=\"%s\" bType=\"%s\" vtype=\"%d\" dime=\"%s\" path=\"%s\" value=\"%s\" primaryRated=\"%.3f\" secondaryRated=\"%.3f\" />\r\n",
					pFcda->chn_no,pFcda->chn_type,pFcda->ln_type,pFcda->do_type,pFcda->ln_desc,pFcda->doi_desc,pFcda->dai_desc,pFcda->bType,pFcda->vtype,pFcda->dime,pFcda->path.data(),pFcda->value.data(),pFcda->primary_rated,pFcda->secondary_rated);
				pFcda = pDataset->m_Fcda.FetchNext(pos3);
			}

			f.fprintf("\t\t\t</dataset>\r\n");
			pDataset = pIed->m_Dataset.FetchNext(pos2);
		}

		//smvout
		stuSclVtIedSmvOut *pSmvOut = pIed->m_SmvOut.FetchFirst(pos2);
		while(pSmvOut)
		{
			f.fprintf("\t\t\t<smv-out smv_no=\"%d\" name=\"%s\" ds_name=\"%s\" datSet=\"%s\" confRev=\"%d\" nofASDU=\"%d\" smpRate=\"%d\" smvID=\"%s\" "
				"multicast=\"%s\" desc=\"%s\" appid=\"0x%X\" mac=\"%s\" vlan=\"%d\" vlan_priority=\"%d\" refreshTime=\"%s\" "
				"sampleRate=\"%s\" sampleSynchronized=\"%s\" security=\"%s\" dataRef=\"%s\" />\r\n",
				pSmvOut->smv_no,pSmvOut->name,pSmvOut->ds_name.data(),pSmvOut->datSet,pSmvOut->confRev,pSmvOut->nofASDU,pSmvOut->smpRate,pSmvOut->smvID,
				pSmvOut->multicast?"true":"false",SString::safeChar(pSmvOut->desc),pSmvOut->appid,pSmvOut->mac,pSmvOut->vlan,pSmvOut->vlan_priority,pSmvOut->refreshTime?"true":"false",
				pSmvOut->sampleRate?"true":"false",pSmvOut->sampleSynchronized?"true":"false",pSmvOut->security?"true":"false",pSmvOut->dataRef?"true":"false");

			pSmvOut = pIed->m_SmvOut.FetchNext(pos2);
		}

		//gooseout
		stuSclVtIedGooseOut *pGooseOut = pIed->m_GooseOut.FetchFirst(pos2);
		while(pGooseOut)
		{
			f.fprintf("\t\t\t<goose-out gse_no=\"%d\" gocb=\"%s\" ds_name=\"%s\" datSet=\"%s\" confRev=\"%d\" name=\"%s\" appid=\"0x%X\" "
				"mac=\"%s\" vlan=\"%d\" vlan_priority=\"%d\" min_time=\"%d\" max_time=\"%d\" />\r\n",
				pGooseOut->gse_no,pGooseOut->gocb,pGooseOut->ds_name.data(),pGooseOut->datSet,pGooseOut->confRev,pGooseOut->name,pGooseOut->appid,
				pGooseOut->mac,pGooseOut->vlan,pGooseOut->vlan_priority,pGooseOut->min_time,pGooseOut->max_time);

			pGooseOut = pIed->m_GooseOut.FetchNext(pos2);
		}

		//smvin
		stuSclVtIedSmvIn *pSmvIn = pIed->m_SmvIn.FetchFirst(pos2);
		while(pSmvIn)
		{
			f.fprintf("\t\t\t<smv-in int_smv_no=\"%d\" ext_iedname=\"%s\" ext_smv_no=\"%d\">\r\n",
				pSmvIn->int_smv_no,pSmvIn->ext_iedname,pSmvIn->ext_smv_no);
			stuSclVtIedSmvInChannel *pChn = pSmvIn->m_Channel.FetchFirst(pos3);
			while(pChn)
			{
				f.fprintf("\t\t\t\t<channel int_chn_no=\"%d\" int_chn_desc=\"%s\" int_path=\"%s\" ext_chn_no=\"%d\" />\r\n",
					pChn->int_chn_no,pChn->int_chn_desc,pChn->int_path,pChn->ext_chn_no);
				pChn = pSmvIn->m_Channel.FetchNext(pos3);
			}

			f.fprintf("\t\t\t</smv-in>\r\n");

			pSmvIn = pIed->m_SmvIn.FetchNext(pos2);
		}

		//goosein
		stuSclVtIedGooseIn *pGooseIn = pIed->m_GooseIn.FetchFirst(pos2);
		while(pGooseIn)
		{
			f.fprintf("\t\t\t<goose-in int_gse_no=\"%d\" ext_iedname=\"%s\" ext_ldname=\"%s\" ext_gse_no=\"%d\">\r\n",
				pGooseIn->int_gse_no,pGooseIn->ext_iedname,pGooseIn->ext_ldname,pGooseIn->ext_gse_no);
			stuSclVtIedGooseInChannel *pChn = pGooseIn->m_Channel.FetchFirst(pos3);
			while(pChn)
			{
				f.fprintf("\t\t\t\t<channel int_chn_no=\"%d\" int_chn_desc=\"%s\" int_path=\"%s\" ext_chn_no=\"%d\" />\r\n",
					pChn->int_chn_no,pChn->int_chn_desc,pChn->int_path,pChn->ext_chn_no);
				pChn = pGooseIn->m_Channel.FetchNext(pos3);
			}

			f.fprintf("\t\t\t</goose-in>\r\n");

			pGooseIn = pIed->m_GooseIn.FetchNext(pos2);
		}
		
		f.fprintf("\t\t</ied>\r\n");
		pIed = m_VirtualTerminal.m_Ied.FetchNext(pos1);
	}
	f.fprintf("\t</ied-list>\r\n");

	stuSimSubnetwork *pSubNet = m_SubNet.FetchFirst(pos1);
	while(pSubNet)
	{
		f.fprintf("\t<subnetwork name=\"%s\" desc=\"%s\" ip=\"%s\" mac=\"%s\" >\r\n",
			pSubNet->name.data(),pSubNet->desc.data(),pSubNet->ip.data(),pSubNet->mac.data());
		stuSclVtIedSmvOut *pSmv = pSubNet->m_Smv.FetchFirst(pos2);
		while(pSmv)
		{
			f.fprintf("\t\t<smv ied_name=\"%s\" smv_no=\"%d\" />\r\n",
				pSmv->ied_name.data(),pSmv->smv_no);
			pSmv = pSubNet->m_Smv.FetchNext(pos2);
		}
		stuSclVtIedGooseOut *pGoose = pSubNet->m_Goose.FetchFirst(pos2);
		while(pGoose)
		{
			f.fprintf("\t\t<goose ied_name=\"%s\" gse_no=\"%d\" />\r\n",
				pGoose->ied_name.data(),pGoose->gse_no);
			pGoose = pSubNet->m_Goose.FetchNext(pos2);
		}

		stuSimMmsSvr *pMmsSvr = pSubNet->m_MmsSvr.FetchFirst(pos2);
		while(pMmsSvr)
		{
			f.fprintf("\t\t<mms_svr iedname=\"%s\" comtrade_path=\"%s\" />\r\n",
				pMmsSvr->iedname.data(),pMmsSvr->comtrade_path.data());
			pMmsSvr = pSubNet->m_MmsSvr.FetchNext(pos2);
		}

		stuSimMmsClient *pMmsClient = pSubNet->m_MmsClient.FetchFirst(pos2);
		while(pMmsClient)
		{
			f.fprintf("\t\t<mms_client iedname=\"%s\" ip=\"%s\" rpt_id=\"%d\" />\r\n",
				pMmsClient->iedname.data(),pMmsClient->ip.data(),pMmsClient->rpt_id);
			pMmsClient = pSubNet->m_MmsClient.FetchNext(pos2);
		}

		stuSimNtpSvr *pNtpSvr = pSubNet->m_NtpSvr.FetchFirst(pos2);
		while(pNtpSvr)
		{
			f.fprintf("\t\t<mms_svr iedname=\"%s\" />\r\n",
				pNtpSvr->iedname.data());
			pNtpSvr = pSubNet->m_NtpSvr.FetchNext(pos2);
		}

		stuSimNtpClient *pNtpClient = pSubNet->m_NtpClient.FetchFirst(pos2);
		while(pNtpClient)
		{
			f.fprintf("\t\t<mms_client iedname=\"%s\" ip=\"%s\" interval_sec=\"%d\" />\r\n",
				pNtpClient->iedname.data(),pNtpClient->ip.data(),pNtpClient->interval_sec);
			pNtpClient = pSubNet->m_NtpClient.FetchNext(pos2);
		}

		{
			stuSimTcpSvr *p = pSubNet->m_TcpSvr.FetchFirst(pos2);
			while(p)
			{
				f.fprintf("\t\t<tcp_svr port=\"%d\" send_len=\"%d\" send_interval_us=\"%d\" />\r\n",
					p->port,p->send_len,p->send_interval_us);
				p = pSubNet->m_TcpSvr.FetchNext(pos2);
			}
		}
		{
			stuSimTcpClient *p = pSubNet->m_TcpClient.FetchFirst(pos2);
			while(p)
			{
				f.fprintf("\t\t<tcp_client ip=\"%s\" port=\"%d\" send_len=\"%d\" send_interval_us=\"%d\" />\r\n",
					p->ip.data(),p->port,p->send_len,p->send_interval_us);
				p = pSubNet->m_TcpClient.FetchNext(pos2);
			}
		}
		{
			stuSimUdpSend *p = pSubNet->m_UdpSend.FetchFirst(pos2);
			while(p)
			{
				f.fprintf("\t\t<udp_send ip=\"%s\" port=\"%d\" send_len=\"%d\" send_interval_us=\"%d\" />\r\n",
					p->ip.data(),p->port,p->send_len,p->send_interval_us);
				p = pSubNet->m_UdpSend.FetchNext(pos2);
			}
		}
		{
			stuSimIcmpSend *p = pSubNet->m_IcmpSend.FetchFirst(pos2);
			while(p)
			{
				f.fprintf("\t\t<icmp_send ip=\"%s\" send_len=\"%d\" send_interval_us=\"%d\" />\r\n",
					p->ip.data(),p->send_len,p->send_interval_us);
				p = pSubNet->m_IcmpSend.FetchNext(pos2);
			}
		}
		{
			stuSimArpSend *p = pSubNet->m_ArpSend.FetchFirst(pos2);
			while(p)
			{
				f.fprintf("\t\t<arp_send send_len=\"%d\" send_interval_us=\"%d\" />\r\n",
					p->send_len,p->send_interval_us);
				p = pSubNet->m_ArpSend.FetchNext(pos2);
			}
		}
		{
			stuSimStorm *p = pSubNet->m_Storm.FetchFirst(pos2);
			while(p)
			{
				f.fprintf("\t\t<storm send_len=\"%d\" send_interval_us=\"%d\" />\r\n",
					p->send_len,p->send_interval_us);
				p = pSubNet->m_Storm.FetchNext(pos2);
			}
		}

		f.fprintf("\t</subnetwork>\r\n");
		pSubNet = m_SubNet.FetchNext(pos1);
	}
	f.fprintf("\t<mms_server>\r\n");
	stuSclVtIed *pSclIed = m_MmsServerIed.FetchFirst(pos2);
	while(pSclIed)
	{
		f.fprintf("\t\t<mms_svr ied_name=\"%s\" />\r\n",pSclIed->name);
		pSclIed = m_MmsServerIed.FetchNext(pos2);
	}

	f.fprintf("\t</mms_server>\r\n");
	f.fprintf("</sim>\r\n");
	f.close();
	return true;
}


