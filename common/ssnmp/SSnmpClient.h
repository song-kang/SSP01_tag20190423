/**
 *
 * 文 件 名 : SSnmp.h
 * 创建日期 : 2015-12-1 13:15
 * 作    者 : 邵凯田(skt001@163.com)
 * 修改日期 : $Date: $
 * 当前版本 : $Revision: $
 * 功能描述 : SNMP客户端操作接口类
 * 修改记录 : 
 *            $Log: $
 *
 * Ver  Date        Author  Comments
 * ---  ----------  ------  -------------------------------------------
 * 001	2015-12-1	邵凯田　创建文件
 *
 **/
#ifndef __SSNMP_CLIENT_H__
#define __SSNMP_CLIENT_H__

#ifdef WIN32
  #pragma comment(lib,"libeay32.lib")
  #pragma comment(lib,"ssleay32.lib")
  #pragma comment(lib,"ws2_32.lib")
  #ifdef  SSNMP_EXPORT_DLL
    #define SSNMP_EXPORT __declspec(dllexport)
  #else //SSNMP_EXPORT_DLL
    #define SSNMP_EXPORT __declspec(dllimport)
    #ifndef NDEBUG
      #pragma comment(lib,"ssnmpd.lib")
    #else
      #pragma comment(lib,"ssnmp.lib")
    #endif
  #endif //SSNMP_EXPORT_DLL
#else
  #define SSNMP_EXPORT
#endif//WIN32

#include "SApi.h"
#include "SDatabase.h"

enum SSnmpVersion
{
	SSNMP_VER1=1,
	SSNMP_VER2C,
	SSNMP_VER3,
};

struct SSNMP_EXPORT stuSnmpSwitchInfo
{
	SString desc;//描述
	SString objid;//对象标识
	SString up_time;//上电持续时间
	SString contact;//联系方式
	SString name;//名称
	SString location;//位置
	int if_number;//端口数量
};

struct SSNMP_EXPORT stuSSnmpSwitchPortInfo
{
	stuSSnmpSwitchPortInfo()
	{
		port_id = 0;
		port_speed = 0;
		in_real_flow = 0;
		out_real_flow = 0;
		all_real_flow = 0;
		conn_stat = false;
		crc_error = false;
		short_error = false;
		big_error = false;
		storm_error = false;
		tmp_last_crc_error = 0;//
		tmp_last_short_error = 0;//
		tmp_last_big_error = 0;//
		tmp_last_storm_error = 0;//
	}
	int port_id;
	SString port_name;
	int port_speed;//端口速率，0表示未知
	float in_real_flow;//入口流量bps
	float out_real_flow;//出口流量bps
	float all_real_flow;//总流量bps
	bool conn_stat;//是否有物理连接，true:up,false:down
	bool crc_error;//是否有CRC错误
	bool short_error;//是否有超短帧错误
	bool big_error;//是否有超长帧错误
	bool storm_error;//是否有网络风暴

	int sum_in_bytes;//累计接收字节数
	int sum_in_pkgs;//累计接收包数
	int sum_in_bcast_pkgs;//累计广播包数
	int sum_in_mcast_pkgs;//累计多播包数
	int sum_in_pkts_64;//64字节累计字节数
	int sum_in_pkts_65_127;//65-127字节累计包数
	int sum_in_pkts_128_255;//128-255字节累计包数
	int sum_in_pkts_256_511;//256-511字节累计包数
	int sum_in_pkts_512_1023;//512-1023字节累计包数
	int sum_in_pkts_1024_1518;//1024-1518字节累计包数

	int tmp_last_crc_error;//
	int tmp_last_short_error;//
	int tmp_last_big_error;//
	int tmp_last_storm_error;//

};

struct SSNMP_EXPORT stuSSnmpSwitchPortMacInfo
{
	int port_id;
	SString mac;//:分隔的MAC
	bool is_learned;//是否为学习出来的MAC
	unsigned int ip;//0表示无效（即无法获取对应IP）
};

//交换机描述
//.iso.org.dod.internet.mgmt.mib-2.system.sysDescr.0
#define OID_sysDescr "1.3.6.1.2.1.1.1"
//设备标识
//.iso.org.dod.internet.mgmt.mib-2.system.sysObjectID
#define OID_sysObjectID "1.3.6.1.2.1.1.2"
//交换机上电持续时间
//.iso.org.dod.internet.mgmt.mib-2.system.sysUpTime
#define OID_sysUpTime "1.3.6.1.2.1.1.3"
//联系
//.iso.org.dod.internet.mgmt.mib-2.system.sysContact
#define OID_sysContact "1.3.6.1.2.1.1.4"
//名称
//.iso.org.dod.internet.mgmt.mib-2.system.sysName
#define OID_sysName "1.3.6.1.2.1.1.5"
//位置
//.iso.org.dod.internet.mgmt.mib-2.system.sysLocation
#define OID_sysLocation "1.3.6.1.2.1.1.6"

//取网络端口数量
//.iso.org.dod.internet.mgmt.mib-2.interfaces.ifNumber
#define OID_ifNumber "1.3.6.1.2.1.2.1.0"

//取网络端口序号入口OID
//.iso.org.dod.internet.mgmt.mib-2.interfaces.ifTable.ifEntry.ifIndex
#define OID_ifIndex "1.3.6.1.2.1.2.2.1.1"

//取网络端口名称入口OID
//.iso.org.dod.internet.mgmt.mib-2.interfaces.ifTable.ifEntry.ifDescr
#define OID_ifDescr "1.3.6.1.2.1.2.2.1.2"

//取网络端口速率入口OID
//.iso.org.dod.internet.mgmt.mib-2.interfaces.ifTable.ifEntry.ifSpeed
#define OID_ifSpeed "1.3.6.1.2.1.2.2.1.5"

//取网络端口连接状态入口OID
//.iso.org.dod.internet.mgmt.mib-2.interfaces.ifTable.ifEntry.ifOperStatus
#define OID_ifOperStatus "1.3.6.1.2.1.2.2.1.8"


//端口MAC地址
//.iso.org.dod.internet.mgmt.mib-2.dot1dBridge.dot1dTp.dot1dTpFdbTable.dot1dTpFdbEntry.dot1dTpFdbAddress
#define OID_MACAddr "1.3.6.1.2.1.17.4.3.1.1"
//端口MAC址与端口号关联
//.iso.org.dod.internet.mgmt.mib-2.dot1dBridge.dot1dTp.dot1dTpFdbTable.dot1dTpFdbEntry.dot1dTpFdbPort
#define OID_MACAddrPort "1.3.6.1.2.1.17.4.3.1.2"
//端口MAC来源（学习or静态）
//.iso.org.dod.internet.mgmt.mib-2.dot1dBridge.dot1dTp.dot1dTpFdbTable.dot1dTpFdbEntry.dot1dTpFdbStatus
#define OID_MACAddrPortSrc "1.3.6.1.2.1.17.4.3.1.3"

//端口CRC告警次数
//.iso.org.dod.internet.mgmt.mib-2.rmon.statistics.etherStatsTable.etherStatsEntry.etherStatsCRCAlignErrors
#define OID_CrcError "1.3.6.1.2.1.16.1.1.1.8"

//端口巨帧次数
//.iso.org.dod.internet.mgmt.mib-2.rmon.statistics.etherStatsTable.etherStatsEntry.etherStatsJabbers
#define OID_BigFrameError "1.3.6.1.2.1.16.1.1.1.12"

//端口短帧次数
//.iso.org.dod.internet.mgmt.mib-2.rmon.statistics.etherStatsTable.etherStatsEntry.etherStatsUndersizePkts
#define OID_ShortFrameError "1.3.6.1.2.1.16.1.1.1.9"

//端口累计接收字节数
//.iso.org.dod.internet.mgmt.mib-2.rmon.statistics.etherStatsTable.etherStatsEntry.etherStatsOctets
#define OID_etherStatsOctets "1.3.6.1.2.1.16.1.1.1.4"

//端口累计接收包数
//.iso.org.dod.internet.mgmt.mib-2.rmon.statistics.etherStatsTable.etherStatsEntry.etherStatsPkts
#define OID_etherStatsPkts "1.3.6.1.2.1.16.1.1.1.5"

//广播累计包数
//.iso.org.dod.internet.mgmt.mib-2.rmon.statistics.etherStatsTable.etherStatsEntry.etherStatsBroadcastPkts
#define OID_etherStatsBroadcastPkts "1.3.6.1.2.1.16.1.1.1.6"

//多播累计包数
//.iso.org.dod.internet.mgmt.mib-2.rmon.statistics.etherStatsTable.etherStatsEntry.etherStatsMulticastPkts
#define OID_etherStatsMulticastPkts "1.3.6.1.2.1.16.1.1.1.7"

//64字节累计包数
//.iso.org.dod.internet.mgmt.mib-2.rmon.statistics.etherStatsTable.etherStatsEntry.etherStatsPkts64Octets
#define OID_etherStatsPkts64Octets "1.3.6.1.2.1.16.1.1.1.14"

//65-127字节累计包数
//.iso.org.dod.internet.mgmt.mib-2.rmon.statistics.etherStatsTable.etherStatsEntry.etherStatsPkts65to127Octets
#define OID_etherStatsPkts65to127Octets "1.3.6.1.2.1.16.1.1.1.15"

//128-255字节累计包数
//.iso.org.dod.internet.mgmt.mib-2.rmon.statistics.etherStatsTable.etherStatsEntry.etherStatsPkts128to255Octets
#define OID_etherStatsPkts128to255Octets "1.3.6.1.2.1.16.1.1.1.16"

//256-511字节累计包数
//.iso.org.dod.internet.mgmt.mib-2.rmon.statistics.etherStatsTable.etherStatsEntry.etherStatsPkts256to511Octets
#define OID_etherStatsPkts256to511Octets "1.3.6.1.2.1.16.1.1.1.17"

//512-1023字节累计包数
//.iso.org.dod.internet.mgmt.mib-2.rmon.statistics.etherStatsTable.etherStatsEntry.etherStatsPkts512to1023Octets
#define OID_etherStatsPkts512to1023Octets "1.3.6.1.2.1.16.1.1.1.18"

//1024-1518字节累计包数
//.iso.org.dod.internet.mgmt.mib-2.rmon.statistics.etherStatsTable.etherStatsEntry.etherStatsPkts1024to1518Octets
#define OID_etherStatsPkts1024to1518Octets "1.3.6.1.2.1.16.1.1.1.19"

class SSNMP_EXPORT SSnmpClient
{
public:
	SSnmpClient();
	virtual ~SSnmpClient();

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  全局初始化静态函数
	// 作    者:  邵凯田
	// 创建时间:  2015-12-1 13:54
	// 参数说明:  void
	// 返 回 值:  void
	//////////////////////////////////////////////////////////////////////////
	static void GlobalInit();

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  全局回收静态函数
	// 作    者:  邵凯田
	// 创建时间:  2015-12-1 13:54
	// 参数说明:  void
	// 返 回 值:  void
	//////////////////////////////////////////////////////////////////////////
	static void GlobalExit();

	void SetVersion(SSnmpVersion ver){m_Version = ver;};
	void SetCommunity(SString sCommunityName);
	void SetRetryTimes(int times){m_iRetryTimes = times;};
	void SetTimeoutMs(int ms){m_iTimeoutMs = ms;};
	void SetServerAddress(SString ip,int port=161);
	SString GetServerIp(){return m_sServerIp;};
	int GetServerPort(){return m_iPort;};

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  根据类型名称返回对应的SNMP类型值
	// 作    者:  邵凯田
	// 创建时间:  2016-7-9 15:03
	// 参数说明:  @name为类型名称，全部小写，如：int/octets/ipaddr/oid/timeticks/gauge32/cntr32/cntr64/bits/...
	// 返 回 值:  
	//////////////////////////////////////////////////////////////////////////
	int GetSynTaxByName(SString name);

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  将指定的10毫秒为单元的时间计数，转换为可视的字符串
	// 作    者:  邵凯田
	// 创建时间:  2016-7-11 14:09
	// 参数说明:  @hsec_ticks表示装置上电或复位开始计数的时间，从0开始
	// 返 回 值:  SString
	//////////////////////////////////////////////////////////////////////////
	static SString HSecondsTickets2Str(unsigned long hsec_ticks);

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  新建会话
	// 作    者:  邵凯田
	// 创建时间:  2015-12-1 14:10
	// 参数说明:  void
	// 返 回 值:  true/false
	//////////////////////////////////////////////////////////////////////////
	bool BeginSession();

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  结束会话
	// 作    者:  邵凯田
	// 创建时间:  2015-12-1 14:10
	// 参数说明:  void
	// 返 回 值:  true/false
	//////////////////////////////////////////////////////////////////////////
	bool EndSession();

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  SNMP Get操作
	// 作    者:  邵凯田
	// 创建时间:  2015-12-1 14:12
	// 参数说明:  oids为多个OID字符串，通过','分隔
	//         :  @sValues表示引用返回的结果值，属性串方式,oid=value;多输出时用';'分隔
	//         :  @pRowHexList表示是否将对应列值读取为原始的HEX字符，列表数量与oid数量一致
	// 返 回 值:  >0表示成功，=0表示OID不存在，<0表示失败
	//////////////////////////////////////////////////////////////////////////
	int Get(SString oids,SString &sValues,SValueList<bool> *pRowHexList=NULL);

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  SNMP Get操作，返回到记录对象中
	// 作    者:  邵凯田
	// 创建时间:  2016-7-8 10:52
	// 参数说明:  oids为多个OID字符串，通过','分隔
	//         :  @rec为记录对象
	//         :  @pRowHexList表示是否将对应列值读取为原始的HEX字符，列表数量与oid数量一致
	// 返 回 值:  >0表示成功，=0表示OID不存在，<0表示失败
	//////////////////////////////////////////////////////////////////////////
	int Get(SString oids,SRecord &rec,SValueList<bool> *pRowHexList=NULL);

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  SNMP Set写操作
	// 作    者:  邵凯田
	// 创建时间:  2016-7-8 15:27
	// 参数说明:  oids为多个OID字符串，通过','分隔
	//         :  @sValues表示引用返回的结果值；多内容时用';'分隔，数量与oids对应
	// 返 回 值:  >0表示成功，=0表示OID不存在，<0表示失败
	//////////////////////////////////////////////////////////////////////////
	int Set(SString oids,SString sValues);

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  根据给定的值类型写一个点的值
	// 作    者:  邵凯田
	// 创建时间:  2016-7-9 14:56
	// 参数说明:  @oid表示路径
	//         :  @sValue表示值内容
	//         :  @iValType表示值类型
	// 返 回 值:  >0表示成功，=0表示OID不存在，<0表示失败
	//////////////////////////////////////////////////////////////////////////
	int SetOneWithType(SString oid,SString sValue,int iValType);

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  SNMP GetNext操作
	// 作    者:  邵凯田
	// 创建时间:  2015-12-1 14:12
	// 参数说明:  oid为单个OID字符串
	//         :  @sValues表示引用返回的结果值，属性串方式,oid=value;
	//         :  @bRawHexStr表示是否显示有16进制的格式化字符串
	// 返 回 值:  >0表示成功，=0表示OID不存在，<0表示失败
	//////////////////////////////////////////////////////////////////////////
	int GetNext(SString oid,SString &sValues,bool bRawHexStr=false);

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  SNMP多字段 GetNext操作
	// 作    者:  邵凯田
	// 创建时间:  2016-7-8 9:13
	// 参数说明:  oids为多个OID字符串，通过','分隔
	//         :  @re为结果记录
	//         :  @pRowHexList表示是否将对应列值读取为原始的HEX字符，列表数量与oid数量一致
	// 返 回 值:  >0表示成功返回的记录数量，=0表示OID不存在，<0表示失败
	//////////////////////////////////////////////////////////////////////////
	int GetNextMulti(SString oids,SRecord &rec,SValueList<bool> *pRowHexList=NULL);

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  SNMP遍历指定的OID的内容OID及其值
	// 作    者:  邵凯田
	// 创建时间:  2015-12-2 8:58
	// 参数说明:  @oid为单个OID字符串
	//         :  @sValues表示引用返回的结果值，属性串方式,oid=value;oid2=value2;...
	// 返 回 值:  >0表示值数量，=0表示OID不存在，<0表示失败
	//////////////////////////////////////////////////////////////////////////
	int FetchAll(SString oid,SString &sValues);

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  SNMP遍历指定的OID的值
	// 作    者:  邵凯田
	// 创建时间:  2015-12-2 8:58
	// 参数说明:  @oid为单个OID字符串
	//         :  @sValues表示引用返回的结果值，多个值采用';'分隔，最后一个元素后不放';'
	// 返 回 值:  >0表示值数量，=0表示OID不存在，<0表示失败
	//////////////////////////////////////////////////////////////////////////
	int FetchValue(SString oid,SString &sValues);

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  根据给定的广播IP地址，探索子网内所有的支持SNMP协议的终端
	// 作    者:  邵凯田
	// 创建时间:  2016-7-11 8:47
	// 参数说明:  @bcast_ip为广播地址
	//         :  @slResult为结果IP地址集合,每个地址的格式为:IP地址/端口号
	//         :  @ver表示版本号：V1/V2C/V3
	// 返 回 值:  >=0表示返回的终端数量，<0表示失败
	//////////////////////////////////////////////////////////////////////////
	int SnmpDiscover(SString bcast_ip,SStringList &slResult,SSnmpVersion ver);

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  读取设备描述：1.3.6.1.2.1.1.1
	// 作    者:  邵凯田
	// 创建时间:  2016-7-20 20:02
	// 参数说明:  void
	// 返 回 值:  SString
	//////////////////////////////////////////////////////////////////////////
	SString GetSwitchDescr();

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  取交换机信息
	// 作    者:  邵凯田
	// 创建时间:  2015-12-2 10:21
	// 参数说明:  @pInfo为信息指针
	// 返 回 值:  >0表示成功，=0表示没有有效端口，<0表示失败
	//////////////////////////////////////////////////////////////////////////
	int GetSwitchInfo(stuSnmpSwitchInfo *pInfo);

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  取交换机端口信息
	// 作    者:  邵凯田
	// 创建时间:  2015-12-1 20:44
	// 参数说明:  @pSwitchPortInfo表示引用返回的结果列表指针
	// 返 回 值:  >0表示返回端口数量，=0表示没有有效端口，<0表示失败
	//////////////////////////////////////////////////////////////////////////
	int GetSwitchPortInfo(SPtrList<stuSSnmpSwitchPortInfo>* &pSwitchPortInfo);

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  取交换机端口对应的MAC信息表
	// 作    者:  邵凯田
	// 创建时间:  2015-12-1 20:47
	// 参数说明:  @SwitchPortMacInfo表示引用返回的结果列表
	// 返 回 值:  >0表示返回MAC数量，=0表示没有有效MAC，<0表示失败
	//////////////////////////////////////////////////////////////////////////
	int GetSwitchPortMacInfo(SPtrList<stuSSnmpSwitchPortMacInfo> &SwitchPortMacInfo);

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  取当前会话是否连接，以最后一次读写操作结果为依据，成功表示已连接，失败表示断开
	// 作    者:  邵凯田
	// 创建时间:  2016-7-13 8:37
	// 参数说明:  void
	// 返 回 值:  true表示已连接，false表示已断开
	//////////////////////////////////////////////////////////////////////////
	bool IsConnected(){return m_iConnected==1;};

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  交换机连接上的回调虚函数
	// 作    者:  邵凯田
	// 创建时间:  2016-7-20 18:47
	// 参数说明:  
	// 返 回 值:  
	//////////////////////////////////////////////////////////////////////////
	virtual void OnConnected(){};

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  交换机断开连接的回调虚函数
	// 作    者:  邵凯田
	// 创建时间:  2016-7-20 18:47
	// 参数说明:  
	// 返 回 值:  
	//////////////////////////////////////////////////////////////////////////
	virtual void OnDisconnected(){};
private:
	void SetConnected(bool b);

	SString m_sServerIp;
	int m_iPort;
	SString m_sCommunity;//默认为public
	SSnmpVersion m_Version;
	int m_iRetryTimes;//重试次数
	int m_iTimeoutMs;//超时时间，单位ms
	void *m_pUdpAddress;//UdpAddress*
	void *m_pCommunity;// OctetStr* community
	void *m_pSnmp;//Snmp*
	void *m_pCTarget;//CTarget*
	SPtrList<stuSSnmpSwitchPortInfo> m_SwitchPortInfo;
	int m_portinfo_last_soc;//最后更新时间,秒
	int m_portinfo_last_usec;//最后更新时间，微秒
	SLock m_lock;
	int m_iConnected;//是否已连接上装置， 0表示未连接，1表示已连接，-1表示未知
};


//////////////////////////////////////////////////////////////////////////
// 描    述:  Trap事件回调函数
// 作    者:  邵凯田
// 创建时间:  2016-7-11 9:27
// 参数说明:  @pCbParam表示回调参数
//         :  @hsec_ticks表示以10毫秒为单元的时间计数，以装置上电或复位开始从0累计
//         :  @from表示消息来源的可视文本，格式为:IP地址/端口号
//         :  @notify_id表示消息通知的OID
//         :  @oids表示消息携带的oids，多个oid通过','分隔
//         :  @sValues表示与oids对应的值，多个通过';'分隔
// 返 回 值:  NULL
/*
Trap:
hseconds_ticks:256976888l  29 days, 17:49:28.88
from:193.110.25.70/161
notify_id:1.3.6.1.6.3.1.1.5.3.2
enterprise_id:
oids:1.3.6.1.2.1.2.2.1.1,1.3.6.1.2.1.2.2.1.7,1.3.6.1.2.1.2.2.1.8
values:5;2;2
TRAP PARSE OK: event_desc=端口5中断   values=5
*/
//////////////////////////////////////////////////////////////////////////
typedef void (*CB_SSnmp_Trap)(void* pCbParam,unsigned long hsec_ticks,const char *from,const char *notify_id,const char *enterprise_id,SString oids,SString sValues);

//trap 有标准trap和扩展trap之分
//所谓标准trap就是 notify_id为以下5种的trap
//trap值  notify_id的OID        含义
#define C_Trap_coldStart				"1.3.6.1.6.3.1.1.5.1"
#define C_Trap_warmStart				"1.3.6.1.6.3.1.1.5.2"
#define C_Trap_linkDown					"1.3.6.1.6.3.1.1.5.3"//端口连接断开，允许带后缀
#define C_Trap_linkUp					"1.3.6.1.6.3.1.1.5.4"//端口连接恢复，允许带后缀
#define C_Trap_LinkUpDownChnVal			"1.3.6.1.2.1.2.2.1.1"//对应端口Up、Down的端口号，允许带后缀
#define C_Trap_authenticationFailure	"1.3.6.1.6.3.1.1.5.5"
#define C_Trap_egpNeighborloss			"1.3.6.1.6.3.1.1.5.6"

//如果他的notify_id不是以上的5中那么就是扩展trap
//////////////////////////////////////////////////////////////////////////
// 名    称:  SSnmpTrapListenner
// 作    者:  邵凯田
// 创建时间:  2016-7-11 9:46
// 描    述:  基于SNMP的Trap事件帧听类
//////////////////////////////////////////////////////////////////////////
class SSNMP_EXPORT SSnmpTrapListenner
{
public:
	SSnmpTrapListenner();
	~SSnmpTrapListenner();

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  开始Trap接收侦听，一个进程只能使用一个Trap帧听
	// 作    者:  邵凯田
	// 创建时间:  2016-7-11 9:39
	// 参数说明:  @pCallbackFun表示回调函数指针
	//         :  @pCbParam表示回调参数指针
	// 返 回 值:  >0表示成功，<=0表示失败
	//////////////////////////////////////////////////////////////////////////
	int StartTrapListen(CB_SSnmp_Trap pCallbackFun,void* pCbParam,int trap_port=162);

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  结束Trap事件帧听
	// 作    者:  邵凯田
	// 创建时间:  2016-7-11 9:42
	// 参数说明:  void
	// 返 回 值:  void
	//////////////////////////////////////////////////////////////////////////
	void StopTrapListen();

public:
	CB_SSnmp_Trap m_pTrapFun;
	void* m_pTrapCbParam;
	void* m_pSnmp;

};

#endif//__SSNMP_CLIENT_H__
