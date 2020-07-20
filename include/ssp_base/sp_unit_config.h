/**
 *
 * 文 件 名 : sp_unitconfig.h
 * 创建日期 : 2015-7-20 18:43
 * 作    者 : 邵凯田(skt001@163.com)
 * 修改日期 : $Date: $
 * 当前版本 : $Revision: $
 * 功能描述 : 单元配置文件封闭类
 * 修改记录 : 
 *            $Log: $
 *
 * Ver  Date        Author  Comments
 * ---  ----------  ------  -------------------------------------------
 * 001	2015-7-20	邵凯田　创建文件
 *
 **/

#ifndef __SP_UNIT_CONFIG_H__
#define __SP_UNIT_CONFIG_H__

#include "sp_config_mgr.h"
#include "SInclude.h"
#include "ssp_base_inc.h"

//////////////////////////////////////////////////////////////////////////
// 名    称:  eHardwareType
// 作    者:  邵凯田
// 创建时间:  2015-7-20 19:31
// 描    述:  硬件类型枚举，不同类型硬件其硬件相关自检支持程度和方法也不同
//////////////////////////////////////////////////////////////////////////
enum eHardwareType
{
	HW_COMMON_IPC=1,	//普通工控机
	HW_COMMON_PC,		//普通PC机
	HW_COMMON_SERVER,	//普通服务器
	HW_UK21007,			//UK21007网分主机
	HW_UK21008,			//UK21008工控主机
};

//////////////////////////////////////////////////////////////////////////
// 名    称:  eFileSysType
// 作    者:  邵凯田
// 创建时间:  2015-7-20 19:32
// 描    述:  文件系统类型，针对NFS和裸盘，无法对其进行容量获取
//////////////////////////////////////////////////////////////////////////
enum eFileSysType
{
	FS_EXT2=1,			//EXT2
	FS_EXT3,			//EXT3
	FS_EXT4,			//EXT4
	FS_YAFFS,			//YAFFS
	FS_FAT,				//FAT
	FS_NTFS,			//NTFS
	FS_RAW,				//原始裸盘
};

//////////////////////////////////////////////////////////////////////////
// 名    称:  eNasFileSysType
// 作    者:  邵凯田
// 创建时间:  2015-7-20 19:45
// 描    述:  网络存储器文件系统类型（不是网存上的物理文件系统，而是与本装置的连接类型）
//////////////////////////////////////////////////////////////////////////
enum eNasFileSysType
{
	NFS_NFS=1,			//NFS
	NFS_FTP,			//FTP
};

//////////////////////////////////////////////////////////////////////////
// 名    称:  stuHwCheckSelfInfo
// 作    者:  邵凯田
// 创建时间:  2015-7-20 18:58
// 描    述:  硬件相关预警、告警自检
//////////////////////////////////////////////////////////////////////////
struct SSP_BASE_EXPORT stuHwCheckSelfInfo
{
	bool m_bOpen;						//是否开启监测
	float m_fAlertThreshold;			//预警门槛
	float m_fWarnThreshold;				//告警门槛

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  从指针的配置节点，按指针的属性名称读取自检配置信息
	// 作    者:  邵凯田
	// 创建时间:  2015-7-20 20:03
	// 参数说明:  
	// 返 回 值:  
	//////////////////////////////////////////////////////////////////////////
	void Read(SBaseConfig *p,SString open_attr,SString alert_attr,SString warn_attr)
	{
		m_bOpen = (p->GetAttribute(open_attr).toLower() == "true");
		m_fAlertThreshold = p->GetAttributeF(alert_attr);
		m_fWarnThreshold = p->GetAttributeF(warn_attr);
	}
};

//////////////////////////////////////////////////////////////////////////
// 名    称:  CUnitNetInterface
// 作    者:  邵凯田
// 创建时间:  2015-7-20 19:34
// 描    述:  网络接口定义，一个实例对应一个物理网口，每个网口可0或多个IP和网关
//////////////////////////////////////////////////////////////////////////
class SSP_BASE_EXPORT CUnitNetInterface
{
public:
	CUnitNetInterface();
	~CUnitNetInterface();
	struct stuAddr
	{
		SString m_sIp;						//IP地址
		int m_iMaskSize;					//掩码位数
	};
	struct stuGateway
	{
		SString m_sNetIp;					//子网IP，空表示缺省(default)
		SString m_sGwIp;					//网关IP
	};
//private:
	SString m_sName;						//网络接口名称
	SString m_sDesc;						//网络接口描述
	bool m_bMonitor;						//是否监视网口状态
	bool m_bOptical;						//是否光纤接口
	stuHwCheckSelfInfo m_ChkSelfOptPower;	//光功率自检（m_bOptical为true时有效）
	SPtrList<stuAddr> m_Addrs;				//IP地址
	SPtrList<stuGateway> m_Gateways;		//网关
};

//////////////////////////////////////////////////////////////////////////
// 名    称:  CUnitDisk
// 作    者:  邵凯田
// 创建时间:  2015-7-20 19:35
// 描    述:  存储介质盘，对应磁盘、SSD或Flash
//////////////////////////////////////////////////////////////////////////
class SSP_BASE_EXPORT CUnitDisk
{
public:
	CUnitDisk();
	~CUnitDisk();

	//////////////////////////////////////////////////////////////////////////
	// 名    称:  stuPartition
	// 作    者:  邵凯田
	// 创建时间:  2015-7-20 19:36
	// 描    述:  分区定义
	//////////////////////////////////////////////////////////////////////////
	struct stuPartition
	{
		SString m_sName;					//分区名称
		SString m_sDesc;					//分区描述
		eFileSysType m_FsType;				//文件系统类型
		stuHwCheckSelfInfo m_ChkSelfFree;	//分区空闲容量自检
	};

	SString m_sName;						//存储介质名称
	SString m_sDesc;						//存储介质描述
	SPtrList<stuPartition> m_Parts;			//所有分区
};

//////////////////////////////////////////////////////////////////////////
// 名    称:  CUnitNas
// 作    者:  邵凯田
// 创建时间:  2015-7-20 19:38
// 描    述:  网络存储器
//////////////////////////////////////////////////////////////////////////
class SSP_BASE_EXPORT CUnitNas
{
public:
	CUnitNas();
	~CUnitNas();

	struct stuPartition
	{
		SString m_sName;					//分区名称
		SString m_sDesc;					//分区描述
		SString m_sExtAttr;					//扩展属性
		eNasFileSysType m_FsType;			//文件系统类型
		stuHwCheckSelfInfo m_ChkSelfFree;	//分区空闲容量自检
	};

	SString m_sName;						//网络存储器名称
	SString m_sDesc;						//网络存储器描述
	SPtrList<stuPartition> m_Parts;			//所有分区
};

class SSP_BASE_EXPORT CSpUnit
{
public:
	CSpUnit()
	{
	};
	~CSpUnit()
	{
	};

	int m_iUnitId;//单元唯一标识号
	SString m_sUnitCommIp;//单元间通讯IP
	SString m_sUnitName;//单元名称
	SString m_sUnitDesc;//单元描述
};

//////////////////////////////////////////////////////////////////////////
// 名    称:  CUnitConfig
// 作    者:  邵凯田
// 创建时间:  2015-7-20 19:50
// 描    述:  单元管理类
//////////////////////////////////////////////////////////////////////////
class SSP_BASE_EXPORT CUnitConfig : public CConfigBase
{
public:
	CUnitConfig();
	virtual ~CUnitConfig();

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  加载指定文件名称的配置文件
	// 作    者:  邵凯田
	// 创建时间:  2015-7-20 19:51
	// 参数说明:  @sPathFile为配置文件全路径
	// 返 回 值:  true表示加载成功，false表示加载失败
	//////////////////////////////////////////////////////////////////////////
	virtual bool Load(SString sPathFile);

//protected:
	stuHwCheckSelfInfo m_ChkSelfTemp;	//温度自检
	stuHwCheckSelfInfo m_ChkSelfHumi;	//温度自检
	stuHwCheckSelfInfo m_ChkSelfCpu;	//CPU利用率自检
	stuHwCheckSelfInfo m_ChkSelfPwr[4];	//装置电源自检
	stuHwCheckSelfInfo m_ChkSelfAllDisk;//所有受控分区总容量自检

//private:
	SString m_sVersion;					//配置文件版本号
	int m_iUnitId;						//单元ID
	SString m_sSystemType;				//系统型号
	SString m_sHmiStyle;				//HMI界面风格
	SString m_sUnitName;				//单元名称
	SString m_sUnitDesc;				//单元描述
	SString m_sThisIp;					//本单元的IP地址
	eHardwareType m_HwType;				//单元硬件类型
	SPtrList<CUnitNetInterface> m_Nets;	//所有网口
	SPtrList<CUnitDisk> m_Disks;		//所有磁盘
	SPtrList<CUnitNas> m_Nass;			//所有网络存储器
	SPtrList<CSpUnit> m_Units;			//所有的单元
	SString m_sUsbStartDev;				//U盘起始设备名，如：sdd
	SStringList m_slAutoRunCmd;			//所有自动运行的命令
};

#endif//__SP_UNIT_CONFIG_H__
