/**
 *
 * 文 件 名 : sim_config.h
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

#ifndef __SIM_CONFIG_H__
#define __SIM_CONFIG_H__
#include "SString.h"
#include "SList.h"
#include "sp_virtual_terminal.h"
#include "SBaseConfig.h"
struct stuSimMmsSvr;
struct stuSimMmsClient;
struct stuSimNtpSvr;
struct stuSimNtpClient;
struct stuSimTcpSvr;
struct stuSimTcpClient;
struct stuSimUdpSend;
struct stuSimIcmpSend;
struct stuSimArpSend;
struct stuSimStorm;


//<subnetwork name="eth1" desc="网口1" ip="193.100.100.1" netmask="255.255.255.0" mac="12:22:33:44:55:66" >
struct stuSimSubnetwork
{
	SString name;
	SString desc;
	SString ip;
	SString netmask;
	SString mac;
	SString type;//类型
	SString local_netcard;//本地网卡名称
	stuSimSubnetwork()
	{
		m_MmsSvr.setAutoDelete(true);
		m_MmsClient.setAutoDelete(true);
		m_NtpSvr.setAutoDelete(true);
		m_NtpClient.setAutoDelete(true);
		m_TcpSvr.setAutoDelete(true);
		m_TcpClient.setAutoDelete(true);
		m_UdpSend.setAutoDelete(true);
		m_IcmpSend.setAutoDelete(true);
		m_ArpSend.setAutoDelete(true);
		m_Storm.setAutoDelete(true);
	}
	~stuSimSubnetwork()
	{
		m_Smv.clear();
		m_Goose.clear();
		m_MmsSvr.clear();
		m_MmsClient.clear();
		m_NtpSvr.clear();
		m_NtpClient.clear();
	}
	void delGooseOut(stuSclVtIedGooseOut* pCur)
	{
		if (pCur == NULL)
		{
			return;
		}
		for(int i = 0; i < m_Goose.count(); i ++)
		{
			stuSclVtIedGooseOut* pGoose = m_Goose.at(i);
			if (pGoose == NULL)
			{
				continue;
			}
			if (pGoose == pCur)
			{
				m_Goose.remove(i);
				return;
			}
		}
	}

	void delSmvOut(stuSclVtIedSmvOut* pCur)
	{
		if (pCur == NULL)
		{
			return;
		}
		for(int i = 0; i < m_Smv.count(); i ++)
		{
			stuSclVtIedSmvOut* pSmv = m_Smv.at(i);
			if (pSmv == NULL)
			{
				continue;
			}
			if (pSmv == pCur)
			{
				m_Smv.remove(i);
				return;
			}
		}
	}

	SPtrList<stuSclVtIedSmvOut> m_Smv;
	SPtrList<stuSclVtIedGooseOut> m_Goose;
	SPtrList<stuSimMmsSvr> m_MmsSvr;
	SPtrList<stuSimMmsClient> m_MmsClient;
	SPtrList<stuSimNtpSvr> m_NtpSvr;
	SPtrList<stuSimNtpClient> m_NtpClient;
	SPtrList<stuSimTcpSvr> m_TcpSvr;
	SPtrList<stuSimTcpClient> m_TcpClient;
	SPtrList<stuSimUdpSend> m_UdpSend;
	SPtrList<stuSimIcmpSend> m_IcmpSend;
	SPtrList<stuSimArpSend> m_ArpSend;
	SPtrList<stuSimStorm> m_Storm;

};


//<smv ied_name="" smv_no="1" />

//<goose ied_name="" gse_no="1" />

//<mms_svr iedname="" comtrade_path="">
struct stuSimMmsSvr
{
	SString iedname;
	SString comtrade_path;
};

//<mms_client iedname="" ip="" rpt_id="1" >
struct stuSimMmsClient
{
	SString iedname;
	SString ip;
	int rpt_id;
};

//<ntp_svr iedname="">
struct stuSimNtpSvr
{
	SString iedname;
};

//<ntp_client iedname="" ip="" interval_sec="10">
struct stuSimNtpClient
{
	SString iedname;
	SString ip;
	int interval_sec;
};

//<tcp_svr port="8000" send_len="1000" send_interval_us="1000" />
struct stuSimTcpSvr
{
	int port;
	int send_len;
	int send_interval_us;
};

//<tcp_client ip="193.100.101.250" port="8000" send_len="1000" send_interval_us="1000" />
struct stuSimTcpClient
{
	SString ip;
	int port;
	int send_len;
	int send_interval_us;
};

//<udp_send ip="193.100.101.250" port="8000" send_len="1000" send_interval_us="1000" />
struct stuSimUdpSend
{
	SString ip;
	int port;
	int send_len;
	int send_interval_us;
};

//<icmp_send ip="193.100.101.250" send_len="1000" send_interval_us="1000" />
struct stuSimIcmpSend
{
	SString ip;
	int send_len;
	int send_interval_us;
};

//<arp_send send_len="1000" send_interval_us="1000" />
struct stuSimArpSend
{
	int send_len;
	int send_interval_us;
};

//<storm send_len="1000" send_interval_us="1000" />
struct stuSimStorm
{
	int send_len;
	int send_interval_us;
};
struct stuSimRunScript
{
	SString name;//脚本名称
	int run_times;//运行次数
	int run_sepms;//运行间隔时间（毫秒）
	SString script;//脚本内容
	
	bool rt_running;//当前是否在运行
	int rt_times;//当前已运行的次数
	bool rt_stop;//是否准备停止运行
};
class CSimConfig
{
public:
	CSimConfig();
	~CSimConfig();

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  加载配置文件
	// 作    者:  邵凯田
	// 创建时间:  2015-12-22 13:35
	// 参数说明:  sFile
	// 返 回 值:  true/false
	//////////////////////////////////////////////////////////////////////////
	bool Load(SString sFile);

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  加载虚端子信息
	// 作    者:  邵凯田
	// 创建时间:  2015-12-23 16:15
	// 参数说明:  
	// 返 回 值:  
	//////////////////////////////////////////////////////////////////////////
	bool LoadSclVt(SBaseConfig *pIedList);

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  保存配置文件
	// 作    者:  邵凯田
	// 创建时间:  2015-12-22 13:52
	// 参数说明:  sFile
	// 返 回 值:  true/false
	//////////////////////////////////////////////////////////////////////////
	bool Save(SString sFile="");

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  根据名称查找指定的子网
	// 作    者:  邵凯田
	// 创建时间:  2016-11-12 10:15
	// 参数说明:  @sub_name表示子网名称
	// 返 回 值:  stuSimSubnetwork*, NULL表示找不到
	//////////////////////////////////////////////////////////////////////////
	stuSimSubnetwork* SearchSubnetworkByName(const char *sub_name);

	inline stuSimRunScript* SearchRunScriptByName(SString name)
	{
		unsigned long pos;
		stuSimRunScript* p = m_Scripts.FetchFirst(pos);
		while(p)
		{
			if(p->name == name)
				return p;
			p = m_Scripts.FetchNext(pos);
		}
		return NULL;
	}

/*	SString m_sScdFile;*/
	CSclVt m_VirtualTerminal;//虚端口信息

	bool m_bOpenMmsServer;//是否打开MMS服务
	bool m_bMmsSvrSimIpAddr;//MMS服务是否模拟IP地址
	SString m_sMmsExtAttribute;
	SPtrList<stuSimSubnetwork> m_SubNet;
	SPtrList<stuSclVtIed> m_MmsServerIed;
	SString m_sCfgFile;
	SPtrList<stuSimRunScript> m_Scripts;

	int m_iEditSn;//编辑序号，每修改一次加1
	int m_iMmsStTimes,m_iMmsMxTimes;
};

#endif//__SIM_CONFIG_H__
