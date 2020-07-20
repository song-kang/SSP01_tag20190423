/**
 *
 * 文 件 名 : SMmsServer.h
 * 创建日期 : 2016-7-23 10:52
 * 作    者 : 邵凯田(skt001@163.com)
 * 修改日期 : $Date: $
 * 当前版本 : $Revision: $
 * 功能描述 : MMS通讯服务类封装类
 * 修改记录 : 
 *            $Log: $
 *
 * Ver  Date        Author  Comments
 * ---  ----------  ------  -------------------------------------------
 * 001	2016-7-23	邵凯田　创建文件
 *
 **/
#ifndef __SMMS_SERVER_H__
#define __SMMS_SERVER_H__

#include "SInclude.h"
#include "SService.h"
#include <map>

#ifdef _WIN32
  #ifdef  SMMS_SERVER_EXPORT_DLL
    #define SMMS_SERVER_EXPORT __declspec(dllexport)
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
    #pragma comment(lib,"ositcps_l.lib")

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
    #pragma comment(lib,"ositcps_ld.lib")
    #pragma comment(lib,"slog_ld.lib")
    #pragma comment(lib,"smpval_ld.lib")
    #pragma comment(lib,"utility_ld.lib")
    #pragma comment(lib,"ositcps_ld.lib")

    #pragma comment(linker,"/nodefaultlib:libcmtd.lib")
    #pragma comment(linker,"/nodefaultlib:libcmt.lib")
#endif//NDEBUG
  #else //SMMS_SERVER_EXPORT_DLL
    #define SMMS_SERVER_EXPORT __declspec(dllimport)
    #ifdef NDEBUG
      #pragma comment(lib,"SMmsServer.lib")
    #else//NDEBUG

      #pragma comment(lib,"SMmsServerd.lib")
    #endif//NDEBUG
  #endif//SMMS_SERVER_EXPORT_DLL
#else//WIN32
  #define SMMS_SERVER_EXPORT
#endif//WIN32

class SMmsServer;

struct SMMS_SERVER_EXPORT stuMmsServerIed
{
	stuMmsServerIed()
	{
		connected = false;
		scl_info = NULL;
	}
	~stuMmsServerIed()
	{
		if(scl_info != NULL)
		{
			delete scl_info;
			scl_info = NULL;
		}
	}
	static stuMmsServerIed* New(SString scl_file,int ied_no,SString ied_name,SString ap_name)
	{
		stuMmsServerIed *p = new stuMmsServerIed();
		p->scl_pathfile = scl_file;
		p->ied_no = ied_no;
		p->ied_name = ied_name;
		p->ap_name = ap_name;
		return p;
	}
	SString scl_pathfile;//SCL文件名称
	int ied_no;
	SString ied_name;
	SString ap_name;//访问点名称，缺省为S1，存储在接入二次设备的通讯参数中
	void * /*SCL_INFO*/ scl_info;
	bool connected;//是否连接中
	SPtrList<void* /*DATA_MAP*/> m_maps;//信息点到模型中叶子DATA_MAP的映射
};

struct SMMS_SERVER_EXPORT stuLeafMap
{
	void* va_ptr;//MVL_VAR_ASSOC*类型的变量内存指针，指向LN对应变量
	void* map_ptr;//DATA_MAP*类型的指针，指向本叶子节点的映射记录
	void* map_head_ptr;//DATA_MAP_HEAD*类型的指针，指向本叶子节点的映射头指针记录
	void* user_ptr;//用户自定义指针

};

#include "SSaxXml.h"
#include "SList.h"
class SMMS_SERVER_EXPORT CScdCommunication : public SSaxXmlEx
{
public:
	//节点结构定义
	struct stuSCL_Communication;
	struct stuSCL_Communication_SubNetwork;
	struct stuSCL_Communication_SubNetwork_ConnectedAP;
	struct stuSCL_Communication
	{
		SConstPtrList<stuSCL_Communication_SubNetwork> m_SubNetwork;
		const char* node_value;
	};
	struct stuSCL_Communication_SubNetwork
	{
		const char* desc;
		const char* type;
		const char* name;
		SConstPtrList<stuSCL_Communication_SubNetwork_ConnectedAP> m_ConnectedAP;
		const char* node_value;
	};
	struct stuSCL_Communication_SubNetwork_ConnectedAP
	{
		const char* iedName;
		const char* apName;
		const char* desc;
		const char* node_value;
	};
	CScdCommunication();
	virtual ~CScdCommunication();
	static stuSaxChoice* CB_SCL(void* cbParam, SSaxAttribute *pAttribute,int iAttributeCount,char* sNodeValue);
	static stuSaxChoice* CB_SCL_Communication(void* cbParam, SSaxAttribute *pAttribute,int iAttributeCount,char* sNodeValue);
	static stuSaxChoice* CB_SCL_Communication_SubNetwork(void* cbParam, SSaxAttribute *pAttribute,int iAttributeCount,char* sNodeValue);
	static stuSaxChoice* CB_SCL_Communication_SubNetwork_ConnectedAP(void* cbParam, SSaxAttribute *pAttribute,int iAttributeCount,char* sNodeValue);
	SConstPtrList<stuSCL_Communication> m_Communication;
private:
	stuSCL_Communication* m_p_SCL_Communication;
	stuSCL_Communication_SubNetwork* m_p_SCL_Communication_SubNetwork;
	stuSCL_Communication_SubNetwork_ConnectedAP* m_p_SCL_Communication_SubNetwork_ConnectedAP;

};

//MMS服务端最大允许的同时打开文件数量
#define C_MAX_MMS_SERVER_FILE_OPENED 64
class SMMS_SERVER_EXPORT SMmsServer : public SService
{
public:
	struct SMMS_SERVER_EXPORT stuSMmsFileInfo
	{
		SString filename;	//file name
		int fsize;			//file size (# bytes)
		bool mtimpres;		//last modified time present,mtime有效时置true,一般当表示目录时为false
		time_t mtime;		//last modified time
	};

	class SMMS_SERVER_EXPORT CMmsMemFile
	{
	public:
		CMmsMemFile()
		{
			m_iReadPos = 0;
			m_pBuffer = NULL;
			m_tOpenTime = m_iTimeoutTimes = 0;
			m_tFileModifyTime = 0;
		}
		~CMmsMemFile()
		{
			if(m_pBuffer != NULL)
				delete[] m_pBuffer;
		}

		BYTE *m_pBuffer;//文件内容
		int m_iLen;//文件长度
		int m_iReadPos;//读取文件的偏移位置，从0开始
		time_t m_tOpenTime;//打开时间
		time_t m_tFileModifyTime;//文件修改时间
		int m_iTimeoutTimes;//超时次数
	};

	SMmsServer();
	virtual ~SMmsServer();
	
	////////////////////////////////////////////////////////////////////////
	// 描    述:  启动服务,虚函数，派生函数中必须先调用此函数
	// 作    者:  邵凯田
	// 创建时间:  2016-7-23 11:11
	// 参数说明:  void
	// 返 回 值:  true表示服务启动成功,false表示服务启动失败
	//////////////////////////////////////////////////////////////////////////
	virtual bool Start();

	////////////////////////////////////////////////////////////////////////
	// 描    述:  停止服务，派生函数中必须先调用此函数
	// 作    者:  邵凯田
	// 创建时间:  2016-7-23 11:11
	// 参数说明:  void
	// 返 回 值:  true表示服务停止成功,false表示服务停止失败
	//////////////////////////////////////////////////////////////////////////
	virtual bool Stop();

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  进度文本虚函数，加载服务时使用
	// 作    者:  邵凯田
	// 创建时间:  2017-4-13 15:10
	// 参数说明:  @sTipText
	// 返 回 值:  void
	//////////////////////////////////////////////////////////////////////////
	virtual void OnProgressText(SString sTipText){S_UNUSED(sTipText);};

	////////////////////////////////////////////////////////////////////////
	// 描    述:  解析IED节点
	// 作    者:  钱程林
	// 创建时间:  2016-7-23 11:11
	// 参数说明:  void
	// 返 回 值:  true表示解析成功,false表示解析失败
	//////////////////////////////////////////////////////////////////////////
	virtual bool OnParseIed(stuMmsServerIed *ied);

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  映射叶子节点用户
	// 作    者:  邵凯田
	// 创建时间:  2016-8-30 8:55
	// 参数说明:  @sIedLdName表示IED+LD名称
	//         :  @sLeafPath表示叶子节点路径
	//         :  @ppLeafMap表示叶子节点的用户映射指针的地址，映射指针为空，用户层决定是否需要映射，需要时创建stuLeafMap对象并填写指针，MMS服务内部不负责释放
	// 返 回 值:  true表示成功映射，false表示无法映射
	//////////////////////////////////////////////////////////////////////////
	virtual bool OnMapLeafToUser(char* sIedLdName, char* sLeafPath, stuLeafMap **ppLeafMap)
	{
// 		static int cnt = 0;
// 		cnt ++;
// 		if(cnt % 1000 == 0)
// 			printf("%d \t LEAF MAP : %s    %s\n",cnt,sIedLdName,sLeafPath);
		return false;
	};
	//////////////////////////////////////////////////////////////////////////
	// 描    述:  读取叶子节点的值
	// 作    者:  邵凯田
	// 创建时间:  2016-8-29 18:33
	// 参数说明:  @sIedLdName表示IED+LD名称
	//         :  @sLeafPath表示叶子节点路径
	//         :  @pLeafMap表示叶子节点的用户映射指针,NULL表示未映射
	//         :  @mvluRdVaCtrl表示读取变量控制指针，实际类型为MVLU_RD_VA_CTRL*， 但该类型不需要用户层解析，用于存储数据使用
	//         :  @reason表示触发原因，0周期
	// 返 回 值:  true/false, true表示读取成功，并已将值写入mvluRdVaCtrl指定的值空间,false表示失败 
	//////////////////////////////////////////////////////////////////////////
	virtual bool OnReadLeafValue(const char* sIedLdName, const char* sLeafPath, stuLeafMap *pLeafMap, void *mvluRdVaCtrl, int reason = 0){return false;};

// 	//////////////////////////////////////////////////////////////////////////
// 	// 描    述:  OnReadLeafValue内使用，对变量进行赋值
// 	// 作    者:  邵凯田
// 	// 创建时间:  2016-9-1 15:12
// 	// 参数说明:  @mvluRdVaCtrl表示读取变量控制指针，实际类型为MVLU_RD_VA_CTRL*，与OnReadLeafValue中同名参数一致
// 	// 返 回 值:  true表示赋值成功，false表示赋值失败
// 	//////////////////////////////////////////////////////////////////////////
// 	bool FillReadVar(void *mvluRdVaCtrl,char *pValStr);
// 	bool FillReadVar(void *mvluRdVaCtrl,int val);
// 	bool FillReadVar(void *mvluRdVaCtrl,float val);
// 	bool FillReadVar(void *mvluRdVaCtrl,double val);
// 	bool FillReadVar(void *mvluRdVaCtrl,int soc,int usec);

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  读取映射对应的叶子节点的值，并以字符串格式返回
	// 作    者:  邵凯田
	// 创建时间:  2016-9-2 9:18
	// 参数说明:  @pLeafMap表示映射指针
	// 返 回 值:  SString表示值内容
	//////////////////////////////////////////////////////////////////////////
	SString GetLeafVarValue(stuLeafMap *pLeafMap);

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  根据DATA_MAP指针读取节点的值，并以字符串格式返回
	// 作    者:  邵凯田
	// 创建时间:  2017-4-21 13:44
	// 参数说明:  @pDataMap为节点的DATA_MAP*指针
	// 返 回 值:  SString表示值内容
	//////////////////////////////////////////////////////////////////////////
	SString GetLeafVarValueByDM(void *pDataMap);

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  读取指定路径的节点值，如果是复合对象，返回所有子节点带大括号的嵌套值（子节点顺序以模型中顺序为准）
	// 作    者:  邵凯田
	// 创建时间:  2017-4-21 8:55
	// 参数说明:  @sMmsPath为MMS全路径：CL2223CTRL/GGIO26$ST$Alm2$stVal
	// 返 回 值:  SString表示值内容
	//////////////////////////////////////////////////////////////////////////
	SString GetLeafVarValueByPath(SString sMmsPath);
	
	//////////////////////////////////////////////////////////////////////////
	// 描    述:  读取指定路径的节点值，如果是复合对象，返回所有子节点带大括号的嵌套值（子节点顺序以模型中顺序为准）
	// 作    者:  邵凯田
	// 创建时间:  2017-4-21 8:57
	// 参数说明:  @sIedLd表示IED名+逻辑节点：CL2223CTRL
	//         :  @sLeafPath表示叶子节点名：GGIO26$ST$Alm2$stVal
	// 返 回 值:  SString表示值内容
	//////////////////////////////////////////////////////////////////////////
	SString GetLeafVarValueByPath(SString sIedLd,SString sLeafPath);

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  设置映射对应的叶子节点的值
	// 作    者:  邵凯田
	// 创建时间:  2016-9-1 18:41
	// 参数说明:  @pLeafMap
	// 返 回 值:  true表示赋值成功，false表示赋值失败
	//////////////////////////////////////////////////////////////////////////
	bool SetLeafVarValue(stuLeafMap *pLeafMap,char *pValStr);
	bool SetLeafVarValue(stuLeafMap *pLeafMap,int val);
	bool SetLeafVarValue(stuLeafMap *pLeafMap,float val);
	bool SetLeafVarValue(stuLeafMap *pLeafMap,double val);
	bool SetLeafVarValue(stuLeafMap *pLeafMap,int soc,int usec);

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  设置DATA_MAP指针对应的叶子节点的值
	// 作    者:  邵凯田
	// 创建时间:  2017-4-21 13:48
	// 参数说明:  @pDataMap为DATA_MAP*指针
	//         :  @pValStr为值字符串，多节点通过'{}'和','分隔
	// 返 回 值:  true表示赋值成功，false表示赋值失败
	//////////////////////////////////////////////////////////////////////////
	bool SetLeafVarValueByDM(void* pDataMap,char *pValStr);


	//////////////////////////////////////////////////////////////////////////
	// 描    述:  根据指定的路径，设置节点的值内容
	// 作    者:  邵凯田
	// 创建时间:  2017-4-21 9:08
	// 参数说明:  @sMmsPath表示MMS节点全路径
	//         :  @sVal表示值内容，如果是针对非叶子节点，值为大括号加逗号分隔的值串，顺序应与读取返回的顺序一致
	// 返 回 值:  true表示赋值成功，false表示赋值失败
	//////////////////////////////////////////////////////////////////////////
	bool SetLeafVarValueByPath(SString sMmsPath, SString sVal);

	bool SetLeafVarValueForDgt(SString sMmsPath, int val, SDateTime dt, SString &sErr);
	bool SetLeafVarValueForAna(SString sMmsPath, float val, SDateTime dt, SString &sErr);
	bool SetLeafVarValueForCtlDgt(SString sMmsPath, int val, SDateTime dt, SString &sErr);

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  写一个新的报告
	// 作    者:  邵凯田
	// 创建时间:  2016-9-2 9:10
	// 参数说明:  @pLeafMap为叶子节点映射指针
	//         :  @val表示值
	// 返 回 值:  true表示报告节点赋值成功，false表示赋值失败
	//////////////////////////////////////////////////////////////////////////
	bool NewReportPush(stuLeafMap *pLeafMap,char *pValStr);
	bool NewReportPush(stuLeafMap *pLeafMap,int val);
	bool NewReportPush(stuLeafMap *pLeafMap,float val);
	bool NewReportPush(stuLeafMap *pLeafMap,double val);
	bool NewReportPush(stuLeafMap *pLeafMap,int soc,int usec);

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  处理新的报告，通过扫描报告，使所有NewReportPush更改的报告发出
	// 作    者:  邵凯田
	// 创建时间:  2016-9-2 9:10
	// 参数说明:  void
	// 返 回 值:  void
	//////////////////////////////////////////////////////////////////////////
	void NewReportProc();

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  写叶子节点的值
	// 作    者:  邵凯田
	// 创建时间:  2016-8-29 18:36
	// 参数说明:  @sIedLdName表示IED+LD名称
	//         :  @sLeafPath表示叶子节点路径
	//         :  @pLeafMap表示叶子节点的用户映射指针,NULL表示未映射
	//         :  @mvluWrVaCtrl表示读取变量控制指针，实际类型为MVLU_WR_VA_CTRL*， 但该类型不需要用户层解析，用于存储数据使用
	// 返 回 值:  true/false, true表示写成功，false表示写失败
	//////////////////////////////////////////////////////////////////////////
	virtual bool OnWriteLeafValue(const char* sIedLdName, const char* sLeafPath, stuLeafMap *pLeafMap, void *mvluWrVaCtrl){return true;};


	//////////////////////////////////////////////////////////////////////////
	// 描    述:  取服务端文件目录内容
	// 作    者:  邵凯田
	// 创建时间:  2016-09-02 10:04
	// 参数说明:  @sPath为目录名称
	//         :  @slFiles这目录内容
	//         :  @bNeedToSort表示是否需要排序，true排序，false不排序
	// 返 回 值:  >0表示内容数量，<0表示失败, =0表示内容为空
	//////////////////////////////////////////////////////////////////////////
	virtual int OnServerDirectory(SString sPath,SPtrList<stuSMmsFileInfo> &slFiles,bool bNeedToSort=false);

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  读取服务端文件
	// 作    者:  邵凯田
	// 创建时间:  2016-09-02 10:10
	// 参数说明:  @sFileName表示服务端文件名称
	// 返 回 值:  CMmsMemFile*, 表示加载到内存的文件内容，NULL表示文件打开或读取失败
	//////////////////////////////////////////////////////////////////////////
	virtual CMmsMemFile* OnReadServerFile(SString sFileName);


	//////////////////////////////////////////////////////////////////////////
	// 描    述:  写入服务端文件成功后的回调
	// 作    者:  邵凯田
	// 创建时间:  2016-09-02 10:12
	// 参数说明:  @sFileName表示服务端文件名称
	//         :  @pBuf表示文件内容缓冲区
	//         :  @iLen表示内容长度（字节）
	// 返 回 值:  >0表示写入成功，=0表示写入0，<0表示失败
	//////////////////////////////////////////////////////////////////////////
	virtual int OnWriteServerFile(SString sFileName/*,unsigned char* pBuf,int iLen*/);

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  删除服务端文件
	// 作    者:  邵凯田
	// 创建时间:  2016-09-02 10:12
	// 参数说明:  @sFileName表示服务端文件名称，相对于文件服务根路径，如有子目录应包含子目录
	// 返 回 值:  >0表示删除成功，=0表示文件不存在，<0表示删除失败
	//////////////////////////////////////////////////////////////////////////
	virtual int OnDeleteServerFile(SString sFileName);

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  修改服务端文件名称
	// 作    者:  邵凯田
	// 创建时间:  2016-9-5 10:30
	// 参数说明:  @sOldFileName表示原文件名称，相对于文件服务根路径
	//         :  @sNewFileName表示新文件名称，相对于文件服务根路径
	// 返 回 值:  >0表示改名成功，=0表示原文件不存在,=-1表示新文件已存在，其它<0表示改名失败
	//////////////////////////////////////////////////////////////////////////
	virtual int OnRenameServerFile(SString sOldFileName,SString sNewFileName);

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  程序启动读取CFG文件时触发调用，
	// 作    者:  钱程林
	// 创建时间:  2016-9-5 10:30
	// 参数说明:  @key属性
	//         : @value属性值
	// 返 回 值:  void
	//////////////////////////////////////////////////////////////////////////
	virtual void OnStartupCfgRead(SString key, SString value) {}

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  解析SCD文件，未删除scl_info前会遍历DAI节点
	// 作    者:  钱程林
	// 创建时间:  2016-9-5 10:30
	// 参数说明:  @iedName IED名称
	// 返 回 值:  void
	//////////////////////////////////////////////////////////////////////////
	virtual void OnMapDaiToUser(const char *iedName, const char *inst, const char *varName, const char *flattened, const char *val, const char *sAddr) {};

	virtual bool OnReturnSelect(char *ref, char *st, bool ret) { return false; };
	virtual bool OnReturnOper(char *ref, char *st, bool ret) { return false; };
	virtual bool OnReturnCancel(char *ref, char *st, bool ret) { return false; };
	//////////////////////////////////////////////////////////////////////////
	// 描    述:  检索指定的叶子映射指针
	// 作    者:  邵凯田
	// 创建时间:  2016-9-1 14:09
	// 参数说明:  @sIedLdName表示IED+LD名称
	//         :  @sLeafPath表示叶子节点路径
	// 返 回 值:  stuLeafMap*， NULL表示未找到有效的映射记录
	//////////////////////////////////////////////////////////////////////////
	stuLeafMap* SearchLeafMap(const char* sIedLdName, const char* sLeafPath);

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  通过路径检索指定的叶子映射指针
	// 作    者:  邵凯田
	// 创建时间:  2017-4-20 15:19
	// 参数说明:  @sMmsPath表示MMS路径
	// 返 回 值:  stuLeafMap*， NULL表示未找到有效的映射记录
	//////////////////////////////////////////////////////////////////////////
	stuLeafMap* SearchLeafMap(const char* sMmsPath);

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  通过路径检索指定的叶子映射指针，如果指定路径找不到，则找其所有子节点
	// 作    者:  邵凯田
	// 创建时间:  2017-4-21 8:41
	// 参数说明:  @sMmsPath表示MMS路径
	//         :  @leafs为引用返回的叶子指针引用队列,类型为DATA_MAP*
	// 返 回 值:  表示返回节点或子节点指针的数量，0表示找不到，<0表示失败
	//////////////////////////////////////////////////////////////////////////
	int SearchLeafMapWithChild(const char* sMmsPath,SPtrList<void> &leafs);

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  设置文件服务的根目录
	// 作    者:  邵凯田
	// 创建时间:  2016-8-29 18:39
	// 参数说明:  @sPath为目录名
	// 返 回 值:  void
	//////////////////////////////////////////////////////////////////////////
	void SetRootPath(SString sPath);

	inline void lock(){m_Lock.lock();};
	inline void unlock(){m_Lock.unlock();};

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  取映射节点对应的IED_LD名称
	// 作    者:  邵凯田
	// 创建时间:  2016-9-3 9:59
	// 参数说明:  @map为映射指针
	// 返 回 值:  char*, ""表示无效指针
	//////////////////////////////////////////////////////////////////////////
	char* GetLeafMapIedLdName(stuLeafMap *map);

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  取映射节点对应的节点名称
	// 作    者:  邵凯田
	// 创建时间:  2016-9-3 9:59
	// 参数说明:  @map为映射指针
	// 返 回 值:  char*, ""表示无效指针
	//////////////////////////////////////////////////////////////////////////
	char* GetLeafMapLeaf(stuLeafMap *map);

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  添加IED信息，添加后由SMmmsServer内部负责释放，应在Start函数之前完成所有IED、AP的添加
	// 作    者:  邵凯田
	// 创建时间:  2016-8-29 18:39
	// 参数说明:  @pIed
	// 返 回 值:  void
	//////////////////////////////////////////////////////////////////////////
	void AddIed(stuMmsServerIed *pIed);

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  通过SCD文件添加IED访问点，应在Start函数之前完成本函数调用
	// 作    者:  邵凯田
	// 创建时间:  2016-9-1 15:30
	// 参数说明:  @sScdFile为SCD文件全路径 
	// 返 回 值:  >0表示解析SCD并添加IED访问点成功，=0表示SCD解析成功但其中没有有效的MMS访问点,<0表示失败
	//////////////////////////////////////////////////////////////////////////
	int AddIedByScd(SString sScdFile);

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  设备控制操作的全局使能
	// 作    者:  邵凯田
	// 创建时间:  2017-4-7 15:36
	// 参数说明:  @bSelectEn表示选择的全局使能
	//         :  @bOperEn表示执行的全局使能
	//         :  @bCancelEn表示取消的全局使能
	// 返 回 值:  void
	//////////////////////////////////////////////////////////////////////////
	void SetGlobalCtrl_En(bool bSelectEn,bool bOperEn,bool bCancelEn);


	//////////////////////////////////////////////////////////////////////////
	// 描    述:  将叶子加入报告暂存队列
	// 作    者:  邵凯田
	// 创建时间:  2016-9-2 9:14
	// 参数说明:  @map为报告对应的叶子节点
	// 返 回 值:  void
	//////////////////////////////////////////////////////////////////////////
	void PushNewReport(stuLeafMap *map);

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  设置服务绑定的IP地址，默认为0.0.0.0
	// 作    者:  邵凯田
	// 创建时间:  2016-9-2 9:14
	// 参数说明:  @ip为地址
	// 返 回 值:  void
	//////////////////////////////////////////////////////////////////////////
	void SetBindIPAddr(SString ip);

private:
	//////////////////////////////////////////////////////////////////////////
	// 描    述:  MMS服务端主循环线程
	// 作    者:  邵凯田
	// 创建时间:  2016-7-23 11:11
	// 参数说明:  this
	// 返 回 值:  void*
	//////////////////////////////////////////////////////////////////////////
	static void* ThreadMainLoop(void* lp);

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  映射所有叶子节点
	// 作    者:  邵凯田
	// 创建时间:  2016-9-2 9:13
	// 参数说明:  void
	// 返 回 值:  void
	//////////////////////////////////////////////////////////////////////////
	void MapLeaf();


private:
	int m_iPort;//服务端端口号
// 	bool m_bQuit;//是否准备退出服务
// 	int m_iThreads;//当前运行的线程数量
	SLock m_Lock;//线程锁
	SPtrList<stuLeafMap> m_NewReportPush;//
protected:

public:
	SString m_sRootPath;//文件服务的根文件系统路径
	SPtrList<stuMmsServerIed> m_Ieds;//准备打开的IED+AP
	SPtrList<stuSMmsFileInfo> m_LastDirResult;//最后一次列文件目录内容
	SString m_sLastDirPath;//最后一次列文件目录的目录名，ca_name有效时（分处发送）使用上次的缓存的结果
	CMmsMemFile *m_MemFilePtr[C_MAX_MMS_SERVER_FILE_OPENED];
	int m_iMemFileId;//下一个文件序号0 ~ C_MAX_MMS_SERVER_FILE_OPENED-1
	std::map<std::string, SString> m_mapAddr;
};

#endif//__SMMS_SERVER_H__
