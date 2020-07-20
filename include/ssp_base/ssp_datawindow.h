/**
 *
 * 文 件 名 : Ssp_Datawindow.h
 * 创建日期 : 2015-7-14 9:26
 * 作    者 : 邵凯田(skt001@163.com)
 * 修改日期 : $Date: $
 * 当前版本 : $Revision: $
 * 功能描述 : Datawindow相关对象封装类
 * 修改记录 : 
 *            $Log: $
 *
 * Ver  Date        Author  Comments
 * ---  ----------  ------  -------------------------------------------
 * 001	2015-7-14	邵凯田　创建文件
 *
 **/

#ifndef __SSP_DATAWINDOW_H__
#define __SSP_DATAWINDOW_H__

#include "SList.h"
#include "SApi.h"
#include "SXmlConfig.h"
#include "db/SDatabaseOper.h"
#include "sp_config_mgr.h"
#include "ssp_database.h"
#include "ssp_base_inc.h"

class CSsp_DatawindowMgr;
class CSsp_Reference;

//引用的有效性时间，秒
#define C_REF_TIMEOUT_S 3

//取数据窗口配置实例
#define GET_DW_CFG ((CSsp_DatawindowMgr*)CConfigMgr::GetConfig(SPCFG_DATA_WINDOW))

class CSsp_Dataset;
//内存库数据集内容改变的触发通知
typedef BYTE* (*OnMdbDatasetTrgCallback)(void* cbParam, CMdbClient *pMdbClient,CSsp_Dataset *pDataset, eMdbTriggerType eType,int iTrgRows,int iRowSize,BYTE *pTrgData);

//////////////////////////////////////////////////////////////////////////
// 名    称:  CSsp_DwColumn
// 作    者:  邵凯田
// 创建时间:  2015-7-14 9:43
// 描    述:  数据窗口列定义类
//////////////////////////////////////////////////////////////////////////
class SSP_BASE_EXPORT CSsp_DwColumn
{
public:
	CSsp_DwColumn();
	CSsp_DwColumn(SBaseConfig *pColCfg);
	~CSsp_DwColumn();

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  将当前对象克隆一份并返回指针
	// 作    者:  邵凯田
	// 创建时间:  2015-11-13 15:43
	// 参数说明:  void
	// 返 回 值:  CSsp_DwColumn*
	//////////////////////////////////////////////////////////////////////////
	CSsp_DwColumn* Clone();

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  将指定的数据库值转换成为系统可视化值
	// 作    者:  邵凯田
	// 创建时间:  2015-11-9 16:13
	// 参数说明:  @sValue为可视值
	// 返 回 值:  SString
	//////////////////////////////////////////////////////////////////////////
	SString GetConvDb2Sys(SString sValue);

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  将指定的系统可视化值转换成为数据库值
	// 作    者:  邵凯田
	// 创建时间:  2015-11-9 16:13
	// 参数说明:  @sValue为可视值
	// 返 回 值:  SString
	//////////////////////////////////////////////////////////////////////////
	SString GetConvSys2Db(SString sValue);

	int m_iIdx;//序号，从0开始顺序编号
	SString m_sName;//列显示名称
	SString m_sDbName;//列在数据库中的名称
	SString m_sConv;//列转换方法，""表示不转换
	stuTableField *m_pMdbField;//对应内存表中的字段，仅用于内存库
	int m_iWidth;//列显示宽度
	int m_iType;//列数值类型，0:字符串;1:整数;2:浮点数;
	bool m_bPKey;//是否为主键字段
	bool m_bIsNull;//是否允许为空，true表示允许为空,false表示不允许为空，默认为true,m_bPKey或m_bUniqueKey为true时必定为false
	bool m_bUniqueKey;//是否为唯一键,true表示唯一键，false表示非唯一键，默认为false,m_bPKey为true时必定为true
	int m_iValLen;//列数据长度，浮点数时表示整数长度
	int m_iValDecLen;//列数据的小数长度，浮点数时有效
	SString m_sExtAttr;//表示扩展属性如:min=1;max=100;step=1;
	CSsp_Reference *m_pRef;//关联的引用,NULL表示不关联引用
	SString sExtAttr;//扩展属性
};

struct SSP_BASE_EXPORT stuDwCondition
{
	int iConditionSn;//条件序号，每个数据集从1开始顺序编号，不可重复
	SString sCmpValue;//比较值，根据不同的条件类型及比较类型，组合成为最终的条件比较值（有比较方式或需含比较符）
};

//////////////////////////////////////////////////////////////////////////
// 名    称:  CSsp_Datawindow
// 作    者:  邵凯田
// 创建时间:  2015-7-14 9:42
// 描    述:  数据窗口类
//////////////////////////////////////////////////////////////////////////
class SSP_BASE_EXPORT CSsp_Dataset
{
public:
	struct SSP_BASE_EXPORT stuTriggerParam
	{
		OnMdbDatasetTrgCallback pCallback;
		void *cbParam;
	};
	friend class CSsp_DatawindowMgr;
	CSsp_Dataset();
	virtual ~CSsp_Dataset();

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  根据当前数据集克隆一个新的数据集对象
	// 作    者:  邵凯田
	// 创建时间:  2015-11-13 15:31
	// 参数说明:  void
	// 返 回 值:  CSsp_Dataset*
	//////////////////////////////////////////////////////////////////////////
	CSsp_Dataset* Clone();
	
	//////////////////////////////////////////////////////////////////////////
	// 描    述:  取记录集指针
	// 作    者:  邵凯田
	// 创建时间:  2015-7-14 11:57
	// 参数说明:  @pRs表示目标记录集,NULL表示不提供目标记录集（直接使用对象内置记录值，不保证线程安全性）；
	//         :  @sWhere表示替换条件
	// 返 回 值:  SRecrodset*,NULL表示失败
	//////////////////////////////////////////////////////////////////////////
	SRecordset* GetRecordset(SRecordset *pRs=NULL,SString sWhere="");

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  根据会话参数修复动态条件，修改本对象的SQL语言
	// 作    者:  邵凯田
	// 创建时间:  2015-11-13 16:02
	// 参数说明:  void
	// 返 回 值:  void
	//////////////////////////////////////////////////////////////////////////
	void PrepareDynamicCondition();

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  取内部数据集，如果数据超时则重新加载
	// 作    者:  邵凯田
	// 创建时间:  2015-8-12 9:41
	// 参数说明:  @iTimeoutSec表示超时秒数,0表示立即加载
	//         :  @sWhere表示替换条件
	// 返 回 值:  SRecordset*
	//////////////////////////////////////////////////////////////////////////
	SRecordset* GetInnerRecordset(int iTimeoutSec=10,SString sWhere="");

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  直接提取数据集，不加锁，也不重载数据
	// 作    者:  邵凯田
	// 创建时间:  2015-12-4 10:21
	// 参数说明:  
	// 返 回 值:  
	//////////////////////////////////////////////////////////////////////////
	inline SRecordset* GetInnerRecordsetDirect(){return &m_Recordset;};

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  取列数量
	// 作    者:  邵凯田
	// 创建时间:  2015-7-14 12:17
	// 参数说明:  void
	// 返 回 值:  int
	//////////////////////////////////////////////////////////////////////////
	inline int GetColumnCount(){return m_Columns.count();};

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  取指定序号的列指针
	// 作    者:  邵凯田
	// 创建时间:  2015-7-14 12:18
	// 参数说明:  @idx为序号，从0开始编号
	// 返 回 值:  CSsp_DwColumn*，NULL表示不存在
	//////////////////////////////////////////////////////////////////////////
	inline CSsp_DwColumn* GetColumn(int idx){return m_Columns[idx];};

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  取引用中sql语句
	// 作    者:  褚冰冰
	// 添加时间： 2015-7-28 18:20
	//////////////////////////////////////////////////////////////////////////
	inline SString GetSql(){return m_sSql;};

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  测试当前数据集是否为动态数据集
	// 作    者:  邵凯田
	// 创建时间:  2015-11-13 16:00
	// 参数说明:  void
	// 返 回 值:  true/false
	//////////////////////////////////////////////////////////////////////////
	inline bool IsDynamicDataset(){return m_sSql.find("{@SESSION:") >= 0;};

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  取数据集对应的描述名称
	// 作    者:  邵凯田
	// 创建时间:  2015-8-11 9:13
	// 参数说明:  void
	// 返 回 值:  SString
	//////////////////////////////////////////////////////////////////////////
	inline SString GetDesc(){return m_sDesc;};

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  初始化内存库的数据集，注册触发回调并加载初始数据
	// 作    者:  邵凯田
	// 创建时间:  2015-8-6 14:12
	// 参数说明:  void
	// 返 回 值:  void
	//////////////////////////////////////////////////////////////////////////
	void InitMdb();

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  释放MDB资源，包括回调函数和数据集内容
	// 作    者:  邵凯田
	// 创建时间:  2015-8-6 14:33
	// 参数说明:  void
	// 返 回 值:  void
	//////////////////////////////////////////////////////////////////////////
	void ReleaseMdb();

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  注册数据集变更触发回调函数
	// 作    者:  邵凯田
	// 创建时间:  2015-8-6 14:44
	// 参数说明:  @pFun为回调函数
	//         :  @cbParam为回调参数，通常为调用对象的this指针
	// 返 回 值:  void
	//////////////////////////////////////////////////////////////////////////
	void RegisterTriggerCallback(OnMdbDatasetTrgCallback pFun,void *cbParam);

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  删除数据集变更触发回调函数
	// 作    者:  邵凯田
	// 创建时间:  2015-8-6 14:44
	// 参数说明:  @pFun为回调函数
	//         :  @cbParam为回调参数，通常为调用对象的this指针
	// 返 回 值:  void
	//////////////////////////////////////////////////////////////////////////
	void RemoveTriggerCallback(OnMdbDatasetTrgCallback pFun,void *cbParam);

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  清除所有回调
	// 作    者:  邵凯田
	// 创建时间:  2015-8-17 11:20
	// 参数说明:  void
	// 返 回 值:  void
	//////////////////////////////////////////////////////////////////////////
	void ClearTriggerCallback(){m_Callbacks.clear();};

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  从指定的记录集查找对应原始数据行的的记录指针
	// 作    者:  邵凯田
	// 创建时间:  2015-8-6 15:18
	// 参数说明:  @pRs为记录集指针，NULL表示内置记录集
	//         :  @pRowData为原始行数据（即内存结构体），一般用于触发数据定位数据行
	// 返 回 值:  SRecord*,NULL表示未找到
	//////////////////////////////////////////////////////////////////////////
	SRecord* SearchRecordByPrimKey(SRecordset *pRs,BYTE *pRowData);

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  取指针记录集原始行对应的主键字符串值，多主键用逗号分隔
	// 作    者:  邵凯田
	// 创建时间:  2015-8-7 15:11
	// 参数说明:  @pRs为记录集指针，NULL表示内置记录集
	//         :  @pRowData为原始行数据（即内存结构体），一般用于触发数据定位数据行
	// 返 回 值:  SString
	//////////////////////////////////////////////////////////////////////////
	SString GetPrimKeyByRawRecord(SRecordset *pRs,BYTE *pRowData);

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  从指定的记录集查找对应的主键值行的记录指针
	// 作    者:  邵凯田
	// 创建时间:  2015-8-6 16:33
	// 参数说明:  @pRs为记录集指针，NULL表示内置记录集
	//         :  @sPrimKeyValues为主键值字符中，多主键时该值为逗号分隔（结尾不加）
	// 返 回 值:  SRecord*，NULL表示未找到
	//////////////////////////////////////////////////////////////////////////
	SRecord* SearchRecordByPrimKey(SRecordset *pRs,SString &sPrimKeyValues);

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  从指定的记录集查找对应的主键行，需要在外部加锁
	// 作    者:  邵凯田
	// 创建时间:  2015-8-17 10:12
	// 参数说明:  @pRs为记录集指针，NULL表示内置记录集
	//         :  @oid表示唯一键值，对应表中第一列的int型字段
	// 返 回 值:  SRecord*，NULL表示未找到
	//////////////////////////////////////////////////////////////////////////
	SRecord* SearchRecordByOid(SRecordset *pRs,unsigned int oid,int &rowidx);


	//////////////////////////////////////////////////////////////////////////
	// 描    述:  是否当前内部数据是脏的，即需要上层在使用内部数据集前先将其刷新
	// 作    者:  邵凯田
	// 创建时间:  2015-8-15 15:21
	// 参数说明:  void
	// 返 回 值:  bool
	//////////////////////////////////////////////////////////////////////////
	inline bool IsInnerRsIsDirty(){return m_bInnerRsIsDirty;};

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  将内部数据集置为脏数据
	// 作    者:  邵凯田
	// 创建时间:  2015-8-15 15:22
	// 参数说明:  void
	// 返 回 值:  void
	//////////////////////////////////////////////////////////////////////////
	void SetInnerRsIsDirty(){m_bInnerRsIsDirty = true;};

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  设置当前数据集的动态查询条件
	// 作    者:  邵凯田
	// 创建时间:  2015-10-29 14:09
	// 参数说明:  @condition为已生效条件的队列，未生效条件不能包含在队列中,该队列不应设置为自动释放，
	//         :  调用后队列内容将被移动到类中，并由类负责释放
	// 返 回 值:  void
	//////////////////////////////////////////////////////////////////////////
	void SetCondition(SPtrList<stuDwCondition> &condition);
	
	inline void LockInnerRs(){m_InnerRsLock.lock();};
	inline void UnlockInnerRs(){m_InnerRsLock.unlock();};
	inline SString GetTableName(){return m_sTableName;};

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  设置数据集是否自动变换列数据
	// 作    者:  邵凯田
	// 创建时间:  2015-11-12 19:23
	// 参数说明:  @b
	// 返 回 值:  void
	//////////////////////////////////////////////////////////////////////////
	inline void SetAutoColumnTransform(bool b){m_bAutoColumnTransform = b;};
	inline bool GetAutoColumnTransform(){return m_bAutoColumnTransform;};

	inline SString GetClsName(){return m_sClsName;};
	inline SString GetName(){return m_sName;};
	inline int GetType(){return m_iType;};
	inline SString GetExtAttribute(){return m_sExtAttr;};
	SString m_sExtAttr;//扩展属性

private:
	//////////////////////////////////////////////////////////////////////////
	// 描    述:  针对内存数据库时的记录更新回调处理函数，包括记录更新、插入、删除和表截断
	// 作    者:  邵凯田
	// 创建时间:  2015-8-6 13:55
	// 参数说明:  @cbParam为当前记录集的对象指针
	//         :  @sTable为表名
	//         :  @eType为触发类型
	//         :  @iTrgRows为触发行数
	//         :  @iRowSize为每行的记录长度
	//         :  @pTrgData为触发数据内容，总有效长度为iTrgRows*iRowSize
	// 返 回 值:  NULL
	//////////////////////////////////////////////////////////////////////////
	static BYTE* OnMdbTrgCallback(void* cbParam, SString &sTable, eMdbTriggerType eType,int iTrgRows,int iRowSize,BYTE *pTrgData);

	stuDwCondition* SearchConditon(int con_sn);
protected:
	SString m_sClsName;//分类名称
	SString m_sName;
	SString m_sDesc;
	SString m_sSql;//{$Where$}为动态替换条件
	int m_iType;//0:fixed;1:mdb;2:db
	SString m_sTableName;//mdb模式使用
	CSsp_DatawindowMgr *m_pDataWindowMgr;
	SRecordset m_Recordset;//记录集内容
	time_t m_tRsUpdateSOC;//记录集更新时间
	bool m_bRef;//是否是引用
	bool m_bInnerRsIsDirty;//是否内部数据集是脏数据，即需要在显示时刷新数据
	int m_iTransformType;//变换类型：
							//0表示不转置
							//1表示带标题行转置
							//2表示仅数据行转置
							//3表示交叉变换
	SString m_sCrossRefName;//交叉数据集引用名，空表示无交叉配置
	int m_iCrossRefColumnNo;//用于交叉的列序号，从0开始，-1表示无效，transform_type=3时有效
	int m_iCrossDataColumnNo;//被交叉显示的数据列序号，该列将作为新生成的动态交叉列内容，从0开始，-1表示无效，transform_type=3时有效
	bool m_bCrossColumnProcessed;//是否已经处理完成交叉表的动态列
	int m_iCrossRawColumnCnt;//交叉列之前的原始列数，去除数据列和交叉列
	bool m_bAutoColumnTransform;//是否自动变换列数据，如果有变换需求的话

	SPtrList<CSsp_DwColumn> m_Columns;//列定义
	SPtrList<CSsp_DwColumn> m_PrimKeyColumns;//主键列
	SPtrList<stuTriggerParam> m_Callbacks;//回调函数列表
	SPtrList<stuTableField> m_MdbFields;//内存库中对应表的字段信息
	SPtrList<stuDwCondition> m_DwConditons;//数据窗口的动态条件队列
	SLock m_InnerRsLock;//内置数据集的共享访问锁
	CMdbClient *m_pMdbTrgClient;//触发用的内存库客户端指针
	bool m_bWithAggregate;//是否为聚合查询
};

//////////////////////////////////////////////////////////////////////////
// 名    称:  CSsp_Reference
// 作    者:  邵凯田
// 创建时间:  2015-7-14 9:43
// 描    述:  引用类
//////////////////////////////////////////////////////////////////////////
class SSP_BASE_EXPORT CSsp_Reference : public CSsp_Dataset
{
public:
	CSsp_Reference();
	virtual ~CSsp_Reference();

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  根据引用关键字返回引用值，如找不到对应关键字时则直接返回关键字
	// 作    者:  邵凯田
	// 创建时间:  2015-8-5 17:33
	// 参数说明:  @sRefKey为引用关键字
	// 返 回 值:  SString
	//////////////////////////////////////////////////////////////////////////
	SString GetRefValue(SString sRefKey);
};

//////////////////////////////////////////////////////////////////////////
// 名    称:  CSsp_DatawindowMgr
// 作    者:  邵凯田
// 创建时间:  2015-7-14 9:43
// 描    述:  数据窗口管理类
//////////////////////////////////////////////////////////////////////////
class SSP_BASE_EXPORT CSsp_DatawindowMgr : public CConfigBase
{
public:
	friend class CSsp_Dataset;
	CSsp_DatawindowMgr();
	virtual ~CSsp_DatawindowMgr();

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  设置内存数据库的操作类
	// 作    者:  邵凯田
	// 创建时间:  2015-7-14 12:02
	// 参数说明:  @pDbOper为数据库操作类，含数据库连接池
	// 返 回 值:  void
	//////////////////////////////////////////////////////////////////////////
	void SetMdbOper(SDatabaseOper *pDbOper){m_pMdbOper = pDbOper;};

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  设置历史数据库的操作类
	// 作    者:  邵凯田
	// 创建时间:  2015-7-14 12:02
	// 参数说明:  @pDbOper为数据库操作类，含数据库连接池
	// 返 回 值:  void
	//////////////////////////////////////////////////////////////////////////
	void SetDbOper(SDatabaseOper *pDbOper){m_pDbOper = pDbOper;};

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  从配置文件加载DataWindow配置
	// 作    者:  邵凯田
	// 创建时间:  2015-7-14 10:13
	// 参数说明:  @sFile为文件名
	// 返 回 值:  true表示加载成功，false表示加载失败
	//////////////////////////////////////////////////////////////////////////
	virtual bool Load(SString sFile);

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  从配置类加载DataWindow配置
	// 作    者:  邵凯田
	// 创建时间:  2015-7-14 10:15
	// 参数说明:  @pDW为数据窗口的配置实例指针
	// 返 回 值:  true表示加载成功，false表示加载失败
	//////////////////////////////////////////////////////////////////////////
	bool LoadCfg(SBaseConfig *pDW);

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  从数据库加载DataWindow配置
	// 作    者:  邵凯田
	// 创建时间:  2015-11-2 18:49
	// 参数说明:  void
	// 返 回 值:  true表示加载成功，false表示加载失败
	//////////////////////////////////////////////////////////////////////////
	bool LoadByDb();

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  检索指定名称的引用实例指针
	// 作    者:  邵凯田
	// 创建时间:  2015-7-14 11:06
	// 参数说明:  @sRefName为引用名称
	// 返 回 值:  CSsp_Reference*，NULL表示未找到
	//////////////////////////////////////////////////////////////////////////
	CSsp_Reference* SearchReference(SString sRefName);

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  检索指定名称的数据集实例指针
	// 作    者:  邵凯田
	// 创建时间:  2015-7-14 11:55
	// 参数说明:  @sDsName为数据集名称
	// 返 回 值:  CSsp_Dataset*，NULL表示未找到
	//////////////////////////////////////////////////////////////////////////
	CSsp_Dataset* SearchDataset(SString sDsName);

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  清除所有内容
	// 作    者:  邵凯田
	// 创建时间:  2015-8-17 11:21
	// 参数说明:  void
	// 返 回 值:  void
	//////////////////////////////////////////////////////////////////////////
	void Clear();

	inline SPtrList<CSsp_Reference>* GetReferencePtr(){return  &m_References;};
	inline SPtrList<CSsp_Dataset>* GetDatasetPtr(){return &m_Datasets;};

private:
	//////////////////////////////////////////////////////////////////////////
	// 描    述:  脏数据集自动刷新线程
	// 作    者:  邵凯田
	// 创建时间:  2015-12-21 16:06
	// 参数说明:  this
	// 返 回 值:  NULL
	//////////////////////////////////////////////////////////////////////////
	static void* ThreadRefresh(void *lp);

	int m_iState;//Thread state 0:run, 1:stoping, 2:stoped
	SPtrList<CSsp_Reference> m_References;//引用集合
	SPtrList<CSsp_Dataset> m_Datasets;//数据集集合
	SDatabaseOper *m_pMdbOper;//内存库数据库操作类指针
	SDatabaseOper *m_pDbOper;//历史库数据库操作类指针
	
};

#endif//__SSP_DATAWINDOW_H__
