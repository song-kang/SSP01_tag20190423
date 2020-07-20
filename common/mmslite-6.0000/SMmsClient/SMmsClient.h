/**
 *
 * 文 件 名 : SMmsClient.h
 * 创建日期 : 2015-9-16 19:22
 * 作    者 : 邵凯田(skt001@163.com)
 * 修改日期 : $Date: $
 * 当前版本 : $Revision: $
 * 功能描述 : sbase-MMS客户端封装类
 * 修改记录 : 
 *            $Log: $
 *
 * Ver  Date        Author  Comments
 * ---  ----------  ------  -------------------------------------------
 * 001	2015-9-16	邵凯田　创建文件
 *
 **/

#include "SInclude.h"
#include <map>

#ifdef _WIN32
  #ifdef  SMMS_CLIENT_EXPORT_DLL
    #define SMMS_CLIENT_EXPORT __declspec(dllexport)
  #ifdef NDEBUG
    #pragma comment(lib,"winmm.lib")
    #pragma comment(lib,"ws2_32.lib")
    #pragma comment(lib,"Packet.lib")
#ifdef WIN64
	#pragma comment(lib,"sbase64.lib")
#else
	#pragma comment(lib,"sbase.lib")
#endif
    #pragma comment(lib,"advapi32.lib")
    #pragma comment(lib,"asn1_l.lib")
    #pragma comment(lib,"gse_mgmt_l.lib")
    #pragma comment(lib,"mem_l.lib")
    #pragma comment(lib,"mlog_l.lib")
    #pragma comment(lib,"mmsl_l.lib")
    #pragma comment(lib,"mmsle_l.lib")
    #pragma comment(lib,"mvlu_l.lib")
    #pragma comment(lib,"ositpxs_l.lib")
    #pragma comment(lib,"slog_l.lib")
    #pragma comment(lib,"smpval_l.lib")
    #pragma comment(lib,"utility_l.lib")
    
    #pragma comment(linker,"/nodefaultlib:libcmt.lib")
    #pragma comment(linker,"/nodefaultlib:libcmt.lib")
  #else//NDEBUG
    #pragma comment(lib,"winmm.lib")
    #pragma comment(lib,"ws2_32.lib")
    #pragma comment(lib,"Packet.lib")
#ifdef WIN64
	#pragma comment(lib,"sbase64d.lib")
#else
	#pragma comment(lib,"sbased.lib")
#endif
    #pragma comment(lib,"advapi32.lib")
    #pragma comment(lib,"asn1_ld.lib")
    #pragma comment(lib,"gse_mgmt_ld.lib")
    #pragma comment(lib,"mem_ld.lib")
    #pragma comment(lib,"mlog_ld.lib")
    #pragma comment(lib,"mmsl_ld.lib")
    #pragma comment(lib,"mmsle_ld.lib")
    #pragma comment(lib,"mvlu_ld.lib")
    #pragma comment(lib,"ositpxs_ld.lib")
    #pragma comment(lib,"slog_ld.lib")
    #pragma comment(lib,"smpval_ld.lib")
    #pragma comment(lib,"utility_ld.lib")    
    
    #pragma comment(linker,"/nodefaultlib:libcmtd.lib")
    #pragma comment(linker,"/nodefaultlib:libcmt.lib")
  #endif//NDEBUG
  #else //SMMS_CLIENT_EXPORT_DLL
    #define SMMS_CLIENT_EXPORT __declspec(dllimport)
    #ifdef NDEBUG
      #pragma comment(lib,"SMmsClient.lib")
    #else//NDEBUG
      #pragma comment(lib,"SMmsClientd.lib")
    #endif//NDEBUG
  #endif//SMMS_CLIENT_EXPORT_DLL
#else//WIN32
  #define SMMS_CLIENT_EXPORT
#endif//WIN32

//报告触发条件
#define C_SMMS_TRGOPT_DATA	0x40	//数据变化
#define C_SMMS_TRGOPT_Q		0x20	//品质变化
#define C_SMMS_TRGOPT_REF	0x10	//数据刷新
#define C_SMMS_TRGOPT_ZQ	0x08	//周期
#define C_SMMS_TRGOPT_ZZ	0x04	//总召
#define C_SMMS_TRGOPT_ALL	0x7C	//全部触发条件

//原因码
#define REASON_RESERVED					0	//保留原因
#define REASON_DATA_CHANGE				1	//数据变化原因
#define REASON_QUALITY_CHANGE			2	//品质变化原因
#define REASON_DATA_UPDATE				3	//数据更新原因
#define REASON_INTEGRITY				4	//周期数据原因
#define REASON_GENERAL_INTERROGATION	5	//总召唤原因

#define QUALITY_GOOD			0
#define QUALITY_INVALID			1
#define QUALITY_RESERVED		2
#define QUALITY_QUESTIONABLE	3
typedef struct MMS_QUALITY MMS_QUALITY;
struct SMMS_CLIENT_EXPORT MMS_QUALITY 
{
	unsigned short oldData:1;			//旧数据，表示值在给定的时间内不可用	
	unsigned short failure:1;			//故障，表示监视功能已经检出一个内部或外部故障
	unsigned short oscillatory:1;		//抖动
	unsigned short badReference:1;		//坏基准值，表示由于基准值无法校准，值可能不正确
	unsigned short outOfRange:1;		//超值域，表示和这个品质相关联的属性超出了预先定义的值域
	unsigned short overFlow:1;			//溢出，表示和这个品质相关联的属性超出了所能正确表示值的能力
	unsigned short validity:2;			//有效性
	unsigned short res:3;				//保留
	unsigned short operatorBlocked:1;	//操作员闭锁，表示值的刷新已经被闭锁，同时旧数据也应置位
	unsigned short test:1;				//测试，表示值为正在测试，不能用于运行目的
	unsigned short substituted:1;		//源(过程0/被取代1)，给出值的来源信息
	unsigned short inaccurate:1;		//不精确，表示值不满足源要求的精度
	unsigned short inconsistent:1;		//不一致，表示评估功能已经检出不一致
};
typedef union mms_quality_t mms_quality_t;
union mms_quality_t 
{         
	unsigned char byte[2];
	MMS_QUALITY MQ;
}; 

#define TIME_ACCURACY_UNKNOWN	31
#define TIME_ACCURACY_T0		7
#define TIME_ACCURACY_T1		10
#define TIME_ACCURACY_T2		14
#define TIME_ACCURACY_T3		16
#define TIME_ACCURACY_T4		18
#define TIME_ACCURACY_T5		20
typedef struct MMS_TIME_QUALITY MMS_TIME_QUALITY;
struct SMMS_CLIENT_EXPORT MMS_TIME_QUALITY 
{
	unsigned char timeAccuracy:5;	//时间准确度
	unsigned char clockNotSync:1;	//时钟未同步
	unsigned char clockFailue:1;	//时钟故障
	unsigned char leapSecondKnown:1;//已知润秒
};
typedef union mms_time_quality_t mms_time_quality_t;
union mms_time_quality_t 
{         
	unsigned char byte;
	MMS_TIME_QUALITY MTQ;
}; 

class SMmsClient;

//////////////////////////////////////////////////////////////////////////
// 名    称:  stuSMmsReportInfo
// 作    者:  邵凯田
// 创建时间:  2015-9-22 20:10
// 描    述:  报告信息结构定义
//////////////////////////////////////////////////////////////////////////
struct SMMS_CLIENT_EXPORT stuSMmsReportInfo
{
	SString dom_name;
	SString rcb_name;
	SString rptID;//报告ID, example:PROT/LLN0.brcbWarning
	SString buffered;//是否是缓存报告
	int report_en_state;//0:未使能，1:使能成功，2:使能失败
	SString report_en_error;//报告使能错误描述
	int report_id;//报告号,从1开始编号，一般为1~8
	unsigned int IntgPd;//完整性周期(ms)
	unsigned char OptFlds[2];
	unsigned char TrgOps[1];
	unsigned char EntryID[8];
	void* pRCB_INFO;//RCB_INFO*
};

struct SMMS_CLIENT_EXPORT stuSMmsFileInfo
{
	SString filename;	//file name
	int fsize;			//file size (# bytes)
	bool mtimpres;		//last modified time present
	time_t mtime;		//last modified time
};

//////////////////////////////////////////////////////////////////////////
// 名    称:  stuSMmsReportItem
// 作    者:  邵凯田
// 创建时间:  2015-10-9 9:12
// 描    述:  MMS突发报告回调明细项
//////////////////////////////////////////////////////////////////////////
struct SMMS_CLIENT_EXPORT stuSMmsReportItem
{
	void* dataRefName;//类型为MVL_VAR_ASSOC*
	void* dataValue;//类型为MVL_VAR_ASSOC*
	void* Reason;//类型为MVL_VAR_ASSOC*
};

//////////////////////////////////////////////////////////////////////////
// 名    称:  stuSMmsReport
// 作    者:  邵凯田
// 创建时间:  2015-10-9 9:11
// 描    述:  MMS突发报告回调结构
//////////////////////////////////////////////////////////////////////////
struct SMMS_CLIENT_EXPORT stuSMmsReport
{
	void*  RptID;//类型为MVL_VAR_ASSOC*，下同
	void*  OptFlds;
	void*  SqNum;
	void*  TimeOfEntry;
	void*  DatSetNa;
	void*  BufOvfl;
	void*  SubSeqNum;
	void*  MoreSegmentsFollow;
	void*  EntryID;
	void*  ConfRev;
	void*  Inclusion;
	SString sRptID;
	SPtrList<stuSMmsReportItem> Items;

	stuSMmsReport()
	{
		memset(&RptID,0,sizeof(RptID)*11);
		Items.setAutoDelete(true);
	}
	void AddItem(void* ref,void* val,void* q)
	{
		stuSMmsReportItem *p = new stuSMmsReportItem();
		p->dataRefName = ref;
		p->dataValue = val;
		p->Reason = q;
		Items.append(p);
	}
};

typedef enum
{
	TYPE_STRUCT,
	TYPE_ARRAY,
	TYPE_BOOL,
	TYPE_INT8,	
	TYPE_INT16,	
	TYPE_INT32,
	TYPE_INT64,
	TYPE_UINT8,
	TYPE_UINT16,
	TYPE_UINT32,
	TYPE_UINT64,
	TYPE_FLOAT,
	TYPE_DOUBLE,
	TYPE_BIT_STRING,
	TYPE_UTC_TIME,
	TYPE_UTF8_STRING,
	TYPE_BINARY_TIME,
	TYPE_UNKNOWN
} VALUE_TYPE;

class SMMS_CLIENT_EXPORT MMSValue
{
public:
	MMSValue()
	{
		type = TYPE_UNKNOWN;
		strValue = "";
		bValue = false;
		i8Value = 0;
		i16Value = 0;
		i32Value = 0;
		i64Value = 0;
		ui8Value = 0;
		ui16Value = 0;
		ui32Value = 0;
		ui64Value = 0;
		fValue = 0.0;
		dValue = 0.0;
	}

	~MMSValue() {};

	void setName(SString nm) { name = nm; }
	void setType(int m_type) { type = m_type; }
	void setStrValue(SString str_val) { strValue = str_val; }
	void setBoolValue(bool bool_val) { bValue = bool_val; }
	void setInt8Value(signed char i8_val) { i8Value = i8_val; }
	void setInt16Value(signed short i16_val) { i16Value = i16_val; }
	void setInt32Value(signed int i32_val) { i32Value = i32_val; }
	void setInt64Value(INT64 i64_val) { i64Value = i64_val; }
	void setUint8Value(unsigned char ui8_val) { ui8Value = ui8_val; }
	void setUint16Value(unsigned short ui16_val) { ui16Value = ui16_val; }
	void setUint32Value(unsigned int ui32_val) { ui32Value = ui32_val; }
	void setUint64Value(UINT64 ui64_val) { ui64Value = ui64_val; }
	void setFloatValue(float float_val) { fValue = float_val; }
	void setDoubleValue(double double_val) { dValue = double_val; }

	SString			getName() { return name; }
	int				getType() { return type; }
	SString			getStrValue() { return strValue; }
	bool			getBoolValue() { return bValue; }
	signed char		getInt8Value() { return i8Value; }
	signed short	getInt16Value() { return i16Value; }
	signed int		getInt32Value() { return i32Value; }
	INT64			getInt64Value() { return i64Value; }
	unsigned char	getUint8Value() { return ui8Value; }
	unsigned short	getUint16Value() { return ui16Value; }
	unsigned int	getUint32Value() { return ui32Value; }
	UINT64			getUint64Value() { return ui64Value; }
	float			getFloatValue() { return fValue; }
	double			getDoubleValue() { return dValue; }

private:
	SString			name;
	int				type;
	SString			strValue;
	bool			bValue;
	signed char		i8Value;
	signed short	i16Value;
	signed int		i32Value;
	INT64			i64Value;
	unsigned char	ui8Value;
	unsigned short	ui16Value;
	unsigned int	ui32Value;
	UINT64			ui64Value;
	float			fValue;
	double			dValue;
};

struct SMMS_CLIENT_EXPORT stuJournalData
{
	SString ref;
	char	entry_id[8];
	SPtrList<MMSValue> value;
};

struct SMMS_CLIENT_EXPORT stuSMmsDataNode
{
	SString sDesc;//描述
	SString sMmsPath;//LLN0$BR$brcbWarning01$RptID
	void* pValue;//MVL_VAR_ASSOC*, NULL表示为空，第一次使用时创建且不再释放,析构时自动释放
	int iMmsOperId;//操作类型号：MMSOP_READ、MMSOP_WRITE、MMSOP_INFO_RPT、MMSOP_RD_USR_HANDLED ......
	void	*user;	//用户自定义

	stuSMmsDataNode();
	~stuSMmsDataNode();
};

//////////////////////////////////////////////////////////////////////////
// 名    称:  CSMmsDataset
// 作    者:  邵凯田
// 创建时间:  2015-10-9 17:32
// 描    述:  MMS数据集封装类
//////////////////////////////////////////////////////////////////////////
class SMMS_CLIENT_EXPORT CSMmsDataset
{
public:
	CSMmsDataset();
	~CSMmsDataset();

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  添加一条新的数据项条目
	// 作    者:  邵凯田
	// 创建时间:  2015-10-9 17:29
	// 参数说明:  @mmspath为节点全路径(如:PL2202APROT/LLN0$BR$brcbWarning01$RptID)
	// 返 回 值:  stuSMmsDataNode*
	//////////////////////////////////////////////////////////////////////////
	stuSMmsDataNode* AddItem(SString desc,SString mmspath);

	SString m_sIedName;//装置mms名称
	SString m_sDsPath;//数据集路径
	SString m_sDsName;//数据集名称（描述名）
	int m_iDsType;//数据集类型:1-遥信,2-遥测,3-事件,4-告警,5-软压板,6-参数,7-定值,8-故障量,9-通讯工况 ......
	void	*user;//用户自定义
	SPtrList<stuSMmsDataNode> m_Items;
};

/********************************************  
* @brief CSMmsLogicDevice
* @author 宋康
* @date 2016/01/04
* @return MMS逻辑装置封装类
*********************************************/ 
class SMMS_CLIENT_EXPORT CSMmsLogicDevice
{
public:
	CSMmsLogicDevice(SMmsClient*);
	~CSMmsLogicDevice();
 
	/********************************************  
	* @brief 设置数据条目列表
	* @author 宋康
	* @date 2016/01/04
	* @param int iTimeout_s 超时时间（默认10秒）
	* @return true表示设置成功,false表示设置失败
	*********************************************/ 
	bool SetVarNameList(int iTimeout_s=10);

public:
	SMmsClient	*m_client;	//连接实例,MVL_NET_INFO*,NULL表示未连接
	SString		m_sLdPath;		//逻辑装置路径（PL2202APROT）
	SString		m_sLdName;		//逻辑装置名称（描述名）	
	void		*user;			//用户自定义
	SPtrList<SString> m_VarNames;

private:
	/********************************************  
	* @brief 获取数据条目列表
	* @author 宋康
	* @date 2016/01/04
	* @param void * req_info 请求信息，NAMELIST_REQ_INFO*
	* @param void * resp_info 回复信息，NAMELIST_RESP_INFO*
	* @param int iTimeout_s 超时时间
	* @return int 是否获取成功，0表示成功，非0表示不成功
	*********************************************/ 
	int getVarName(void *req_info,void *resp_info,int iTimeout_s);
};

class SMMS_CLIENT_EXPORT SMmsClient
{
public:
	SMmsClient();
	virtual ~SMmsClient();

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  设置MMS服务器地址，必须在启动前进行设置
	// 作    者:  邵凯田
	// 创建时间:  2015-9-16 19:36
	// 参数说明:  @sServerIpA为服务端A网IP地址
	//         :  @sServerIpB为服务端B网IP地址
	//         :  @iPort为服务TCP端口号，缺省为102
	// 返 回 值:  void
	//////////////////////////////////////////////////////////////////////////
	void SetMmsServerAddr(char *sServerIpA,char *sServerIpB="",int iPort=102);

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  初始化MMS客户端运行环境,一个进程仅运行一次即可
	// 作    者:  邵凯田
	// 创建时间:  2015-9-21 18:20
	// 参数说明:  @bSingleThreadMode表示多个客户端实例时是否采用单线程运行模式，true表示单线程模式，false表示多线程模式
	// 返 回 值:  true表示初始化成功，false表示初始化失败
	//////////////////////////////////////////////////////////////////////////
	static bool InitGlobal(bool bSingleThreadMode=false);

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  全局退出时的销毁函数
	// 作    者:  邵凯田
	// 创建时间:  2015-9-22 18:37
	// 参数说明:  void
	// 返 回 值:  true/false
	//////////////////////////////////////////////////////////////////////////
	static bool ExitGlobal();

	////////////////////////////////////////////////////////////////////////
	// 描    述:  启动服务,虚函数，派生函数中必须先调用此函数
	// 作    者:  邵凯田
	// 创建时间:  2015-9-16 19:24
	// 参数说明:  void
	// 返 回 值:  true表示服务启动成功,false表示服务启动失败
	//////////////////////////////////////////////////////////////////////////
	virtual bool Start();

	////////////////////////////////////////////////////////////////////////
	// 描    述:  停止服务，派生函数中必须先调用此函数
	// 作    者:  邵凯田
	// 创建时间:  2015-9-16 19:24
	// 参数说明:  void
	// 返 回 值:  true表示服务停止成功,false表示服务停止失败
	//////////////////////////////////////////////////////////////////////////
	virtual bool Stop();

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  连接指针地址、端口的MMS服务器
	// 作    者:  邵凯田
	// 创建时间:  2015-9-16 19:24
	// 参数说明:  void
	// 返 回 值:  true表示连接成功，false表示连接失败
	//////////////////////////////////////////////////////////////////////////
	bool Connect();

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  断开与服务端的连接
	// 作    者:  邵凯田
	// 创建时间:  2015-9-16 19:26
	// 参数说明:  void
	// 返 回 值:  true表示断开成功,false表示断开失败
	//////////////////////////////////////////////////////////////////////////
	bool DisConnect();

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  取当前实例是否连接到客户端
	// 作    者:  邵凯田
	// 创建时间:  2017-5-26 14:18
	// 参数说明:  void
	// 返 回 值:  true/false
	//////////////////////////////////////////////////////////////////////////
	bool IsConnected(){return (m_pMVL_NET_INFO_A != NULL && (m_pMVL_NET_INFO_B != NULL || m_sServerIpB[0] == '\0'));}
	bool IsConnected_A(){return m_pMVL_NET_INFO_A != NULL;};
	bool IsConnected_B(){return m_pMVL_NET_INFO_B != NULL;};

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  断开A网的连接
	// 作    者:  邵凯田
	// 创建时间:  2015-9-22 14:21
	// 参数说明:  void
	// 返 回 值:  true表示断开成功,false表示断开失败
	//////////////////////////////////////////////////////////////////////////
	bool DisConnect_A();

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  断开B网的连接
	// 作    者:  邵凯田
	// 创建时间:  2015-9-22 14:21
	// 参数说明:  void
	// 返 回 值:  true表示断开成功,false表示断开失败
	//////////////////////////////////////////////////////////////////////////
	bool DisConnect_B();

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  成功连接服务端之后的回调虚函数
	// 作    者:  邵凯田
	// 创建时间:  2015-9-16 19:29
	// 参数说明:  void
	// 返 回 值:  void
	//////////////////////////////////////////////////////////////////////////
	virtual void OnConnected(){};

	virtual void OnConnected_A(){};
	virtual void OnConnected_B(){};

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  从服务端断开的回调虚函数
	// 作    者:  邵凯田
	// 创建时间:  2015-9-16 19:30
	// 参数说明:  void
	// 返 回 值:  void
	//////////////////////////////////////////////////////////////////////////
	virtual void OnDisConnected(){};

	virtual void OnDisConnected_A(){};
	virtual void OnDisConnected_B(){};

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  每秒钟一次的回调，由应用层决定执行定时任务，如通过取服务端标识确定通信是否正常
	// 作    者:  邵凯田
	// 创建时间:  2017-5-26 10:30
	// 参数说明:  void
	// 返 回 值:  void
	//////////////////////////////////////////////////////////////////////////
	virtual void OnSecondTimer(){};

	inline void* GetMVL_NET_INFO_A(){return m_pMVL_NET_INFO_A;};
	inline void* GetMVL_NET_INFO_B(){return m_pMVL_NET_INFO_B;};
	inline void* GetUserInfo_A(){return m_pUserInfo_A;};
	inline void* GetUserInfo_B(){return m_pUserInfo_B;};


	//////////////////////////////////////////////////////////////////////////
	// 描    述:  创建新的报告信息,并添加到报告队列中
	// 作    者:  邵凯田
	// 创建时间:  2015-9-24 8:24
	// 参数说明:  @dom_name表示IED+LD名称
	//		   :  @rptID表示报告ID,example:PROT/LLN0.brcbWarning
	//		   :  @buffered表示是否是缓存报告
	//         :  @rcb_name表示报告控制块名称
	//         :  @report_id表示报告序号，从1开始编号，0表示单实例报告（不需要报告号）
	//         :  @IntgPd表示完整性周期，单位ms
	//         :  @TrgOpt表示触发选项
	// 返 回 值:  stuSMmsReportInfo*
	//////////////////////////////////////////////////////////////////////////
	stuSMmsReportInfo* AddReportInfo(SString dom_name,SString rptID,SString buffered,
		SString rcb_name,int report_id,SString entry_id,int IntgPd=5000,unsigned char TrgOpt=C_SMMS_TRGOPT_ALL);

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  删除报告控制块，并从报告列表删除
	// 作    者:  邵凯田
	// 创建时间:  2015-9-24 9:00
	// 参数说明:  @pRptInfo表示报告指针
	// 返 回 值:  true/false
	//////////////////////////////////////////////////////////////////////////
	bool RemoveReportInfo(stuSMmsReportInfo *pRpt);

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  取报告控制块数量
	// 作    者:  邵凯田
	// 创建时间:  2015-9-22 20:21
	// 参数说明:  void
	// 返 回 值:  int, 为报告控制块的数量
	//////////////////////////////////////////////////////////////////////////
	int GetReportInfoCount(){return m_ReportInfos.count();};

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  取指定序号的报告指针
	// 作    者:  邵凯田
	// 创建时间:  2015-9-22 20:24
	// 参数说明:  @idx为序号，从0开始编号
	// 返 回 值:  stuSMmsReportInfo*,NULL表示指定序号报告不存在
	//////////////////////////////////////////////////////////////////////////
	stuSMmsReportInfo* GetReportInfo(int idx){return m_ReportInfos[idx];};

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  使能所有报告
	// 作    者:  邵凯田
	// 创建时间:  2015-9-23 16:38
	// 参数说明:  @iTimeout_s表示超时的秒数
	// 返 回 值:  <=0表示使能失败，>0表示使能成功的数量，=GetReportInfoCount()表示全部使能成功
	//////////////////////////////////////////////////////////////////////////
	int EnableReport(int iTimeout_s=10);

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  关闭所有报告
	// 作    者:  邵凯田
	// 创建时间:  2015-9-23 16:39
	// 参数说明:  void
	// 返 回 值:  <=0表示关闭失败，>0表示使能成功的数量，=GetReportInfoCount()表示全部使能成功
	//////////////////////////////////////////////////////////////////////////
	int DisableReport(bool bNeedLock=true);

	inline void lock(){m_Lock.lock();};
	inline void unlock(){m_Lock.unlock();};

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  将指定MVL变量转换为字符串并返回
	// 作    者:  邵凯田
	// 创建时间:  2015-9-24 17:02
	// 参数说明:  @pVal为MVL_VAR_ASSOC*
	// 返 回 值:  SString
	//////////////////////////////////////////////////////////////////////////
	SString GetMvlVarText(void *pVal);

	/********************************************  
	* @brief 将指定MVL变量转换为值类队列并返回
	* @author 宋康
	* @date 2015/12/25
	* @param void * pVal MVL_VAR_ASSOC*变量
	* @param SPtrList<MMSValue> & valueList 值类转换队列
	* @return int =0表示转换成功，>0或<0表示转换失败
	*********************************************/ 
	int GetMvlVarValueList(void *pVal,SPtrList<MMSValue> &valueList);

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  取MVL目录内容
	// 作    者:  邵凯田
	// 创建时间:  2015-9-24 18:43
	// 参数说明:  @sPath为目录名称
	//         :  @slFiles这目录内容
	//         :  @bNeedToSort表示是否需要排序，true排序，false不排序
	// 返 回 值:  >0表示内容数量，<0表示失败, =0表示内容为空
	//////////////////////////////////////////////////////////////////////////
	int GetMvlDirectory(SString sPath,SPtrList<stuSMmsFileInfo> &slFiles,bool bNeedToSort=false);

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  下载MMS服务端指定的文件到文件系统
	// 作    者:  邵凯田
	// 创建时间:  2015-9-24 18:47
	// 参数说明:  @sPathFile表示服务端的源文件名
	//         :  @sLocalFile表示目前文件名
	// 返 回 值:  true表示成功,false表示失败
	//////////////////////////////////////////////////////////////////////////
	bool DownMvlFile(SString sPathFile, SString sLocalFile);
	
	//////////////////////////////////////////////////////////////////////////
	// 描    述:  下载MMS服务端指定的文件到内存
	// 作    者:  邵凯田
	// 创建时间:  2015-9-24 18:48
	// 参数说明:  @sPathFile表示服务端的源文件名
	//         :  @pBuffer表示目标缓冲区，当输入为空时将由内部申请空间并引用返回，调用者负责delete释放
	//         :  @iLen表示目标缓冲区长度，当pBuffer非空时表示缓冲区长度，返回时会被置为目标内容的字节数
	// 返 回 值:  true表示成功,false表示失败
	//////////////////////////////////////////////////////////////////////////
	bool DownMvlFileToMem(SString sPathFile, BYTE* &pBuffer, int &iLen);

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  MMS突发报告处理的回调虚函数，使用完报告结构后需要通过delete释放报告指针
	//            可以在派生类的重载函数中同步处理，也可以将其加入报告队列进行缓存处理
	// 作    者:  邵凯田
	// 创建时间:  2015-10-9 9:14
	// 参数说明:  @pMmsRpt表示当前连接对应的突发报告内容
	// 返 回 值:  void
	//////////////////////////////////////////////////////////////////////////
	virtual void OnMmsReportProcess(stuSMmsReport* pMmsRpt){delete pMmsRpt;};

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  添加新的数据集
	// 作    者:  邵凯田
	// 创建时间:  2015-10-9 17:42
	// 参数说明:  @sDsPath为数据集全路径
	//         :  @sDsName为数据集名称
	//         :  @iDsType为数据集类型（参见CSMmsDataset::m_iDsType定义)
	// 返 回 值:  CSMmsDataset*
	//////////////////////////////////////////////////////////////////////////
	CSMmsDataset* AddDataset(SString sIedPath,SString sDsPath,SString sDsName,int iDsType);

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  删除指定的数据集
	// 作    者:  邵凯田
	// 创建时间:  2015-10-9 17:45
	// 参数说明:  @sDsPath为数据集全路径名
	// 返 回 值:  true/false
	//////////////////////////////////////////////////////////////////////////
	bool RemoveDataset(SString sDsPath);

	CSMmsLogicDevice* AddLogicDevice(SString sLdPath,SString sLdName);

	/********************************************  
	* @brief 总召唤所有报告控制块数据
	* @author 宋康
	* @date 2015/12/28
	* @param int iTimeout_s 超时的秒数，默认10秒
	* @return void
	*********************************************/ 
	void ProcessGi(int iTimeout_s=10);

	/********************************************  
	* @brief 总召唤指定报告控制块数据
	* @author 宋康
	* @date 2015/12/28
	* @param SString rcbName 报告控制块名称 超时的秒数
	* @param int iTimeout_s 超时的秒数，默认10秒
	* @return void
	*********************************************/ 
	void ProcessGi(SString rcbName,int iTimeout_s=10);

	/********************************************  
	* @brief 判断原因码是否包含数据变化
	* @author 宋康
	* @date 2015/12/29
	* @param SString reason
	* @return -true表示包含 -false表示不包含
	*********************************************/ 
	bool ReasonIsDchg(SString reason);

	/********************************************  
	* @brief 判断原因码是否包含品质变化
	* @author 宋康
	* @date 2015/12/29
	* @param SString reason
	* @return -true表示包含 -false表示不包含
	*********************************************/ 
	bool ReasonIsQchg(SString reason);

	/********************************************  
	* @brief 判断原因码是否包含数据更新
	* @author 宋康
	* @date 2015/12/29
	* @param SString reason
	* @return -true表示包含 -false表示不包含
	*********************************************/ 
	bool ReasonIsDupd(SString reason);

	/********************************************  
	* @brief 判断原因码是否包含周期变化
	* @author 宋康
	* @date 2015/12/29
	* @param SString reason
	* @return -true表示包含 -false表示不包含
	*********************************************/ 
	bool ReasonIsPeriod(SString reason);

	/********************************************  
	* @brief 判断原因码是否包含总召唤变化
	* @author 宋康
	* @date 2015/12/29
	* @param SString reason
	* @return -true表示包含 -false表示不包含
	*********************************************/ 
	bool ReasonIsGi(SString reason);

	/********************************************  
	* @brief 设置条目标识符
	* @author 宋康
	* @date 2015/12/30
	* @param SString ied_name 装置MMS名称，如：PT1101
	* @param stuSMmsReport * report 回调的报告
	* @return void
	*********************************************/ 
	void SetReportEntryID(stuSMmsReport *report,stuSMmsReportInfo **rptInfo,SString &entry_id);

	/********************************************  
	* @brief 判断品质是否好
	* @author 宋康
	* @date 2015/12/31
	* @param unsigned short quality 品质值
	* @return -true表示是 -false表示否
	*********************************************/ 
	bool QualityIsGood(unsigned short quality);

	/********************************************  
	* @brief 判断品质是否无效
	* @author 宋康
	* @date 2015/12/31
	* @param unsigned short quality 品质值
	* @return -true表示是 -false表示否
	*********************************************/ 
	bool QualityIsInvalid(unsigned short quality);

	/********************************************  
	* @brief 判断品质是否可疑
	* @author 宋康
	* @date 2015/12/31
	* @param unsigned short quality 品质值
	* @return -true表示是 -false表示否
	*********************************************/ 
	bool QualityIsQuestionable(unsigned short quality);

	/********************************************  
	* @brief 判断品质是否溢出
	* @author 宋康
	* @date 2015/12/31
	* @param unsigned short quality 品质值
	* @return -true表示是 -false表示否
	*********************************************/ 
	bool QualityIsOverFlow(unsigned short quality);

	/********************************************  
	* @brief 判断品质是否超值域
	* @author 宋康
	* @date 2015/12/31
	* @param unsigned short quality 品质值
	* @return -true表示是 -false表示否
	*********************************************/ 
	bool QualityIsOutOfRange(unsigned short quality);

	/********************************************  
	* @brief 判断品质是否坏基准值
	* @author 宋康
	* @date 2015/12/31
	* @param unsigned short quality 品质值
	* @return -true表示是 -false表示否
	*********************************************/ 
	bool QualityIsBadReference(unsigned short quality);

	/********************************************  
	* @brief 判断品质是否抖动
	* @author 宋康
	* @date 2015/12/31
	* @param unsigned short quality 品质值
	* @return -true表示是 -false表示否
	*********************************************/ 
	bool QualityIsOscillatory(unsigned short quality);

	/********************************************  
	* @brief 判断品质是否故障
	* @author 宋康
	* @date 2015/12/31
	* @param unsigned short quality 品质值
	* @return -true表示是 -false表示否
	*********************************************/ 
	bool QualityIsFailure(unsigned short quality);

	/********************************************  
	* @brief 判断品质是否旧数据
	* @author 宋康
	* @date 2015/12/31
	* @param unsigned short quality 品质值
	* @return -true表示是 -false表示否
	*********************************************/ 
	bool QualityIsOldData(unsigned short quality);

	/********************************************  
	* @brief 判断品质是否不一致
	* @author 宋康
	* @date 2015/12/31
	* @param unsigned short quality 品质值
	* @return -true表示是 -false表示否
	*********************************************/ 
	bool QualityIsInconsistent(unsigned short quality);

	/********************************************  
	* @brief 判断品质是否不精确
	* @author 宋康
	* @date 2015/12/31
	* @param unsigned short quality 品质值
	* @return -true表示是 -false表示否
	*********************************************/ 
	bool QualityIsInaccurate(unsigned short quality);

	/********************************************  
	* @brief 判断品质是否取代
	* @author 宋康
	* @date 2015/12/31
	* @param unsigned short quality 品质值
	* @return -true表示是 -false表示否
	*********************************************/ 
	bool QualityIsSubstituted(unsigned short quality);

	/********************************************  
	* @brief 判断品质是否测试
	* @author 宋康
	* @date 2015/12/31
	* @param unsigned short quality 品质值
	* @return -true表示是 -false表示否
	*********************************************/ 
	bool QualityIsTest(unsigned short quality);

	/********************************************  
	* @brief 判断品质是否操作员闭锁
	* @author 宋康
	* @date 2015/12/31
	* @param unsigned short quality 品质值
	* @return -true表示是 -false表示否
	*********************************************/ 
	bool QualityIsOperatorBlocked(unsigned short quality);

	/********************************************  
	* @brief 读取定值区个数
	* @author 宋康
	* @date 2016/01/05
	* @param char * dom_name 域名（PT1101LD0）
	* @param int & areaNum 解析后的返回数值
	* @param int timeOut 超时时间，默认10秒
	* @return -true表示读取成功 -false表示读取失败
	*********************************************/ 
	bool ReadSettingAreaNumber(char *dom_name,int &areaNum,int timeOut=10);

	/********************************************  
	* @brief 读取当前定值区区号
	* @author 宋康
	* @date 2016/01/11
	* @param char * dom_name 域名（PT1101LD0）
	* @param int & currentArea 解析后返回的当前定值区区号
	* @param int timeOut 超时时间，默认10秒
	* @return -true表示读取成功 -false表示读取失败 
	*********************************************/ 
	bool ReadCurrentSettingArea(char *dom_name,int &currentArea,int timeOut=10);

	/********************************************  
	* @brief 读取编辑定值区区号
	* @author 宋康
	* @date 2016/01/11
	* @param char * dom_name 域名（PT1101LD0）
	* @param int & currentArea 解析后返回的编辑定值区区号
	* @param int timeOut 超时时间，默认10秒
	* @return -true表示读取成功 -false表示读取失败 
	*********************************************/ 
	bool ReadEditSettingArea(char *dom_name,int &editArea,int timeOut=10);

	/********************************************  
	* @brief 修改当前定值区区号
	* @author 宋康
	* @date 2016/02/02
	* @param char * dom_name 域名（PT1101LD0）
	* @param int currentArea 需修改的定值区区号
	* @param int timeOut 超时时间，默认10秒
	* @return -true表示修改成功 -false表示修改失败 
	*********************************************/ 
	bool WriteCurrentSettingArea(char *dom_name,int currentArea,int timeOut=10);

	/********************************************  
	* @brief 修改编辑定值区区号
	* @author 宋康
	* @date 2016/02/02
	* @param char * dom_name 域名（PT1101LD0）
	* @param int currentArea 需修改的定值区区号
	* @param int timeOut 超时时间，默认10秒
	* @return -true表示修改成功 -false表示修改失败 
	*********************************************/ 
	bool WriteEditSettingArea(char *dom_name,int editArea,int timeOut=10);

	/********************************************  
	* @brief 读取单个数值
	* @author 宋康
	* @date 2016/01/05
	* @param char * dom_name 域名（PT1101LD0）
	* @param char * var_name 值名（LLN0$SG$Enable$setVal）
	* @param SPtrList<MMSValue> & lstValue 解析后的返回数值队列
	* @param int timeOut 超时时间，默认10秒
	* @return -true表示读取成功 -false表示读取失败
	*********************************************/ 
	bool ReadValue(char *dom_name,char *var_name,SPtrList<MMSValue> &lstValue,int timeOut=10);

#define MAX_READ_VALUES_NUMBER	50	//读多数值的数量最大值
	/********************************************  
	* @brief 读取多个数值，数值量不大于(MAX_READ_VALUES_NUMBER=50)
	* @author 宋康
	* @date 2016/01/06
	* @param SPtrList<SString> & dom_name 域名队列（PT1101LD0）
	* @param SPtrList<SString> & var_name 值名队列（LLN0$SG$Enable$setVal）
	* @param SPtrList<SPtrList<MMSValue>> & lstValue 解析后的返回数值队列的队列
	* @param int timeOut 超时时间，默认10秒
	* @return -true表示读取成功 -false表示读取失败
	*********************************************/ 
	bool ReadValues(SPtrList<SString> &dom_name,SPtrList<SString> &var_name,
		SPtrList< SPtrList<MMSValue> > &lstValue,int timeOut=10);

	/********************************************  
	* @brief 写单个数值
	* @author 宋康
	* @date 2016/02/03
	* @param char * dom_name 域名（PT1101LD0）
	* @param char * var_name 值名（LLN0$SG$Enable$setVal）
	* @param char * data 写的数值
	* @param int timeOut 超时时间，默认10秒
	* @return -true表示写值成功 -false表示写值失败
	*********************************************/ 
	bool WriteValue(char *dom_name,char *var_name,char *data,int timeOut=10);

	/********************************************  
	* @brief 固化定值
	* @author 宋康
	* @date 2016/02/03
	* @param char * dom_name 域名（PT1101LD0）
	* @param int timeOut 超时时间，默认10秒
	* @return -true表示固化成功 -false表示固化失败
	*********************************************/ 
	bool CureSetting(char *dom_name,int timeOut=10);

	/********************************************  
	* @brief 遥控操作，包括选控和直控
	* @author 宋康
	* @date 2016/02/01
	* @param char * dom_name 域名（PT1101LD0）
	* @param char * var_name 值名（CSWI1$CO$Pos）
	* @param int ctrlValue 遥控值（0分，1合）
	* @param bool check 遥控检查
	* @param int timeOut 超时时间，默认10秒
	* @return -true表示遥控成功 -false表示遥控失败
	*********************************************/ 
	bool ProcessControl(char *dom_name,char *var_name,int ctrlValue,bool check,int timeOut=10);

	/********************************************  
	* @brief 信号复归/设备复位
	* @author 宋康
	* @date 2016/02/01
	* @param char * dom_name 域名（PT1101LD0）
	* @param char * var_name 值名（LLN0$CO$LEDRs）
	* @return -true表示复归成功 -false表示复归失败
	*********************************************/ 
	bool Reset(char *dom_name,char *var_name);

	/********************************************  
	* @brief 获取装置确认，用于定期测试连接
	* @author 宋康
	* @date 2016/01/26
	* @param int iTimeout_s 超时时间，默认10秒
	* @return -true表示读取成功 -false表示读取失败
	*********************************************/ 
	bool GetIdentify(int iTimeout_s=10);

	bool InitJournal(char *dom_name,char *var_name,unsigned long &delEntries,int timeOut=10);

	bool GetJournalState(char *dom_name,char *var_name,unsigned long &entries,bool &deletable,int timeOut=10);

	bool ReadJournal(char *dom_name,char *var_name,SDateTime &start,SDateTime &end,unsigned char *entryID,
		SPtrList<stuJournalData> &lstJData,bool &follow,int timeOut=10);

	/********************************************  
	* @brief 设置装置内全部LD的数据条目名
	* @author 宋康
	* @date 2016/01/05
	* @return -true表示成功 -false表示失败
	*********************************************/ 
	bool SetIedVarName();

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  设置多线程运行模式，及一个MMSClient会话对象持有一个线程,一个进程可以含有多个MMSClient会话；单线程则一个进程只含有一个主线程
	// 作    者:  邵凯田
	// 创建时间:  2017-5-26 10:17
	// 参数说明:  b:true多线程，false单线程，默认单线程
	// 返 回 值:  NULl
	//////////////////////////////////////////////////////////////////////////
	void SetMultiThreadMode(bool b);

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  有通信回复报文后触发
	// 作    者:  邵凯田
	// 创建时间:  2017-5-26 10:17
	// 参数说明:  void
	// 返 回 值:  NULl
	virtual void onRecvNewPacket(SDateTime dt) {};

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  通信请求否定响应触发
	// 作    者:  邵凯田
	// 创建时间:  2017-5-26 10:17
	// 参数说明:  void
	// 返 回 值:  NULl
	virtual void OnRequestDenial() {};

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  通信请求超时触发
	// 作    者:  邵凯田
	// 创建时间:  2017-5-26 10:17
	// 参数说明:  void
	// 返 回 值:  NULl
	virtual void OnRequestTimeout() {};

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  是否有用户下发的主动操作正在处理，例如召唤定值等，其执行时享有最高优先级，使能报告将等待任务处理完毕后再继续。
	// 作    者:  钱程林
	// 创建时间:  2018-11-16 10:17
	// 参数说明:  void
	// 返 回 值:  若有用户待执行的高优先级任务返回true，没有用户任务返回false
	bool IsUserTaskProcessing();

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  等待系统任务执行完成。如正在使能报告控制块，此时设置用户任务标记为true并调用当前函数，使能过程将停止并将控制权交给用户
	// 作    者:  钱程林
	// 创建时间:  2018-11-16 10:17
	// 参数说明:  timeout 等待超时返回时间，单位毫秒
	// 返 回 值:  若系统任务执行完成返回true，若超时系统任务仍未执行完成返回false
	bool WaitSystemTaskFinish(int timeout = 5000);

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  使能报告回调，每一个报告无论使能成功或失败都会触发回调，子类可以继承实现报告总召唤等功能
	// 作    者:  钱程林
	// 创建时间:  2019-01-16 10:22
	// 参数说明:  rpt 当前使能的报告对象
	//           result 使能结果，true成功，false失败
	//           isLastRpt 是否是最后一个报告
	// 返 回 值:  void
	virtual void OnEnabledRpt(stuSMmsReportInfo *rpt, bool result, bool isLastRpt) {}

private:

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  MMS客户端主线程
	// 作    者:  邵凯田
	// 创建时间:  2015-9-16 19:31
	// 参数说明:  this
	// 返 回 值:  void*
	//////////////////////////////////////////////////////////////////////////
	static void* ThreadMain(void* lp);

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  全局MMS客户端主线程，所有通信实例均使用同一个通信实例
	// 作    者:  邵凯田
	// 创建时间:  2017-5-26 10:17
	// 参数说明:  NULL
	// 返 回 值:  NULl
	//////////////////////////////////////////////////////////////////////////
	static void* ThreadGlobalMmsMain(void *lp);

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  全局MMS客户端定时线程，处理所有设备的连接、重连、全能报告、定时触发等操作
	// 作    者:  邵凯田
	// 创建时间:  2017-5-26 10:26
	// 参数说明:  NULL
	// 返 回 值:  NULL
	//////////////////////////////////////////////////////////////////////////
	static void* ThreadGlobalMmsTimer(void *lp);

	/********************************************  
	* @brief 总召唤报告控制块数据
	* @author 宋康
	* @date 2015/12/28
	* @param stuSMmsReportInfo * rptInfo 报告控制块
	* @param int iTimeout_s 超时的秒数
	* @return void
	*********************************************/ 
	void ProcessReportInfoGi(stuSMmsReportInfo *rptInfo,int iTimeout_s);

	void SDateTimeToMmsBtod(SDateTime *time,void *btod);

private:
	char m_sServerIpA[32];//服务端A网IP地址
	char m_sServerIpB[32];//服务端B网IP地址
	int m_iPort;//服务端端口号
	bool m_bQuit;//是否准备退出服务
	int m_iThreads;//当前运行的线程数量
	void* m_pMVL_NET_INFO_A;//A网连接实例,MVL_NET_INFO*,NULL表示未连接
	void* m_pMVL_NET_INFO_B;//B网连接实例,MVL_NET_INFO*,NULL表示未连接
	void* m_pMVL_NET_INFO_RptEn;//使能报告用的连接实例,MVL_NET_INFO*,NULL表示未连接
	void* m_pDIB_ENTRY_A;//A网的连接DIB条目指针，DIB_ENTRY*,NULL表示未连接
	void* m_pDIB_ENTRY_B;//B网的连接DIB条目指针，DIB_ENTRY*,NULL表示未连接
	void* m_pUserInfo_A;//A网的连接用户信息指针
	void* m_pUserInfo_B;//B网的连接用户信息指针
	SLock m_Lock;//线程锁
	bool m_isSystemTaskProcessing;				// 是否有系统任务正在处理，如使能报告

protected:
	bool m_isUserTaskProcessing;				// 是否有用户任务正在处理
	SPtrList<stuSMmsReportInfo> m_ReportInfos;//待使能的报告信息列表
	SPtrList<CSMmsDataset> m_Datasets;//所有的数据集队列
	SPtrList<CSMmsLogicDevice> m_LogicDevices;//所有逻辑装置队列
};
