/**
 *
 * 文 件 名 : ssp_database.h
 * 创建日期 : 2015-7-29 19:28
 * 作    者 : 邵凯田(skt001@163.com)
 * 修改日期 : $Date: $
 * 当前版本 : $Revision: $
 * 功能描述 : 数据库配置类
 * 修改记录 : 
 *            $Log: $
 *
 * Ver  Date        Author  Comments
 * ---  ----------  ------  -------------------------------------------
 * 001	2015-7-29	邵凯田　创建文件
 *
 **/

#ifndef __SSP_DATABASE_CONFIG_H__
#define __SSP_DATABASE_CONFIG_H__

#include "sp_config_mgr.h"
#include "SDatabase.h"
#include "SDatabaseOper.h"

#include "SMdb.h"
#include "SApi.h"
#include "ssp_base_inc.h"

//取数据库配置实例
#define GET_DB_CFG ((CSsp_Database*)CConfigMgr::GetConfig(SPCFG_DB_CONFIG))

//取历史库操作类
#define DB GET_DB_CFG->GetHisDbOper()

//取内存库操作类
#define MDB GET_DB_CFG->GetMdbOper()

typedef e_SDatabase_Type eSsp_Database_Type;
//#define eSsp_Database_Type e_SDatabase_Type

class SSP_BASE_EXPORT CSsp_Database : public CConfigBase
{
public:
	CSsp_Database();
	virtual ~CSsp_Database();

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  设备连接池大小，应在Load之前进行设置，该参数对主、备及内存库同时生效，默认为3
	// 作    者:  邵凯田
	// 创建时间:  2015-7-30 13:47
	// 参数说明:  @poolsize为池大小，即连接数据库的会话数量
	// 返 回 值:  void
	//////////////////////////////////////////////////////////////////////////
	void SetPoolSize(int poolsize){m_iPoolSize = poolsize;};

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  加载指定的配置文件
	// 作    者:  邵凯田
	// 创建时间:  2015-7-29 19:31
	// 参数说明:  @sPathFile为数据库配置文件名
	// 返 回 值:  true表示加载成功，false表示加载失败
	//////////////////////////////////////////////////////////////////////////
	virtual bool Load(SString sPathFile);

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  取主、备、内存数据库的连接字符串
	// 作    者:  邵凯田
	// 创建时间:  2015-7-29 20:04
	// 参数说明:  void
	// 返 回 值:  SString,用于SDatabase实例的连接参数
	//////////////////////////////////////////////////////////////////////////
	SString GetMasterConnectString();
	SString GetSlaveConnectString();
	SString GetMdbConnectString();

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  取历史数据库操作类指针
	// 作    者:  邵凯田
	// 创建时间:  2015-7-30 14:46
	// 参数说明:  void
	// 返 回 值:  SDatabaseOper*,NULL表示不存在
	//////////////////////////////////////////////////////////////////////////
	inline SDatabaseOper* GetHisDbOper(){return &m_HisDbOper;};

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  取内存数据库操作类指针
	// 作    者:  邵凯田
	// 创建时间:  2015-7-30 14:47
	// 参数说明:  void
	// 返 回 值:  SDatabaseOper*,NULL表示不存在
	//////////////////////////////////////////////////////////////////////////
	inline SDatabaseOper* GetMdbOper(){return &m_MdbOper;};

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  清除所有MDB的内存触发器
	// 作    者:  邵凯田
	// 创建时间:  2016-5-11 16:49
	// 参数说明:  void
	// 返 回 值:  void
	//////////////////////////////////////////////////////////////////////////
	void RemoveAllMdbTrigger();

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  取主数据库类型
	// 作    者:  邵凯田
	// 创建时间:  2016-11-7 15:46
	// 参数说明:  void
	// 返 回 值:  void
	inline eSsp_Database_Type GetMasterType(){return m_MasterDbType;}

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  取备用数据库类型
	// 作    者:  邵凯田
	// 创建时间:  2016-11-7 15:46
	// 参数说明:  void
	// 返 回 值:  void
	inline eSsp_Database_Type GetSlaveType(){return  m_SlaveDbType;}

private:
	//////////////////////////////////////////////////////////////////////////
	// 描    述:  连接池资源的延时释放类
	// 作    者:  邵凯田
	// 创建时间:  2015-7-30 14:05
	// 参数说明:  @lp为this
	// 返 回 值:  NULL
	//////////////////////////////////////////////////////////////////////////
	static void* ThreadDelayFree(void *lp);

	bool	m_bMaster;
	SString m_sMasterType;//mysql/pgsql/oracle/dameng/mdb
	eSsp_Database_Type m_MasterDbType;
	SString m_sMasterHostAddr;
	int		m_iMasterPort;
	SString m_sMasterUser;
	SString m_sMasterPassword;
	SString m_sMasterDbName;

	bool	m_bSlave;
	SString m_sSlaveType;//mysql/pgsql/oracle/dameng/mdb
	eSsp_Database_Type m_SlaveDbType;
	SString m_sSlaveHostAddr;
	int		m_iSlavePort;
	SString m_sSlaveUser;
	SString m_sSlavePassword;
	SString m_sSlaveDbName;

	bool	m_bMdb;
	SString m_sMdbHostAddr;
	int		m_iMdbPort;
	SString m_sMdbUser;
	SString m_sMdbPassword;
	SString m_sMdbDbName;

	SDatabaseOper m_HisDbOper;						 //历史数据库操作类
	SDatabaseOper m_MdbOper;						 //内存数据库操作类
	SDatabasePool<SDatabase> *m_pHisMasterDbPools;	 //主历史数据库连接池
	SDatabasePool<SDatabase> *m_pHisSlaveDbPools;	 //备历史数据库连接池
	SDatabasePool<SDatabase> *m_pMdbDbPools;		 //内存数据库连接池
	SDatabasePool<SDatabase> *m_pOldHisMasterDbPools;//待删除的过期主历史数据库连接池
	SDatabasePool<SDatabase> *m_pOldHisSlaveDbPools; //待删除的过期备历史数据库连接池
	SDatabasePool<SDatabase> *m_pOldMdbDbPools;		 //待删除的过期内存数据库连接池
	int m_iPoolSize;								 //连接池大小
};


#define C_BATCH_DML_MAX_BUFFERSIZE 65536
//数据库执行异步同步到外网时的标记
#define C_SQL_ASYNC_RUN "/*async*/"

class SSP_BASE_EXPORT CSsp_DmlHelper
{
public:
	CSsp_DmlHelper();
	~CSsp_DmlHelper();

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  取当前实例对应的数据库类型
	// 作    者:  邵凯田
	// 创建时间:  2016-12-15 16:14
	// 参数说明:  void
	// 返 回 值:  eSsp_Database_Type
	//////////////////////////////////////////////////////////////////////////
	inline eSsp_Database_Type GetDbType(){return m_eDbType;};

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  将SQL语句中日期类型转为字符串类型字段
	// 作    者:  邵凯田
	// 创建时间:  2016-12-15 16:14
	// 参数说明:  @sField为字段名称
	// 返 回 值:  根据不同的数据库类型而不同
	//////////////////////////////////////////////////////////////////////////
	SString Date2String(SString sField);

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  将SQL语句中字符串改为日期类型
	// 作    者:  邵凯田
	// 创建时间:  2016-12-15 16:14
	// 参数说明:  @sDateTime格式为:yyyy-MM-dd hh:mm:ss
	// 返 回 值:  根据不同的数据库类型而不同
	//////////////////////////////////////////////////////////////////////////
	SString String2Date(SString sDateTime);


protected:
	SDatabaseOper *m_pDbOper;//数据库指针
	SDatabase *m_pDb;
	eSsp_Database_Type m_eDbType;//数据库类型
	bool m_bSqlAsyncRun;//数据库SQL可异步并行执行的SQL前导标记符,true表示可异步,false表示不可异步
};

//////////////////////////////////////////////////////////////////////////
// 名    称:  CSsp_BatchDmlInsert
// 作    者:  邵凯田
// 创建时间:  2016-12-15 10:51
// 描    述:  批量插入的
//////////////////////////////////////////////////////////////////////////
class SSP_BASE_EXPORT CSsp_BatchDmlInsert : public CSsp_DmlHelper
{
public:
	CSsp_BatchDmlInsert();
	~CSsp_BatchDmlInsert();

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  初始化
	// 作    者:  邵凯田
	// 创建时间:  2016-12-15 13:14
	// 参数说明:  @pDb为数据库操作类指针
	//         :  @dbType为数据库类型
	//         :  @sTableName表示表名称
	//         :  @sFields表示字段名，逗号分隔，如:col1,col2,col3
	//         :  @iTimeoutSec表示超时时间，单位:秒
	//         :  @bAsync为数据库SQL可异步并行执行的SQL前导标记符,true表示可异步,false表示不可异步
	// 返 回 值:  void
	//////////////////////////////////////////////////////////////////////////
	void Init(SDatabaseOper *pDb,eSsp_Database_Type dbType,char* sTableName,char* sFields,int iTimeoutSec=5,bool bAsync=true);

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  使用SDatabase初始化
	// 作    者:  邵凯田
	// 创建时间:  2017-8-24 19:14
	// 参数说明:  @pDb为数据库类指针
	//         :  @iTimeoutSec表示超时时间，单位:秒
	//         :  @bAsync为数据库SQL可异步并行执行的SQL前导标记符,true表示可异步,false表示不可异步
	// 返 回 值:  void
	//////////////////////////////////////////////////////////////////////////
	void Init(SDatabase *pDb,char* sTableName,char* sFields,int iTimeoutSec=5,bool bAsync=true);

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  添加一条的值内容，如缓存大小到达，立即执行所有缓存SQL
	// 作    者:  邵凯田
	// 创建时间:  2016-12-15 13:15
	// 参数说明:  @sValues,为对应所有字段的值，逗号分隔，如:1,2,'xx'
	// 返 回 值:  void
	//////////////////////////////////////////////////////////////////////////
	void AddInsertValues(SString sValues);

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  进行一次超时检查，如到达超时时间，立即执行缓存SQL
	// 作    者:  邵凯田
	// 创建时间:  2016-12-15 13:16
	// 参数说明:  @now_soc表示当前时间，0表示不输入时间
	// 返 回 值:  void
	//////////////////////////////////////////////////////////////////////////
	void CheckTimeOut(int now_soc=0);

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  立即执行缓存SQL
	// 作    者:  钱程林
	// 创建时间:  2018-01-19 10:02
	// 参数说明:  void
	// 返 回 值:  void
	//////////////////////////////////////////////////////////////////////////
	void Flush();

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  设置insert SQL的尾部字符，可用于MYSQL的插入转更新，
	//         :  如:on duplicate key update current_val=values(current_val),soc=values(soc) 作为尾部
	//         :        可添加在[ insert into t_oe_element_state (st_sn,current_val,soc) values (1,0,100),(2,1,101) ]的后面实现插入转更新，且可批量
	// 作    者:  邵凯田
	// 创建时间:  2019-4-12 8:28
	// 参数说明:  
	// 返 回 值:  
	//////////////////////////////////////////////////////////////////////////
	void SetSqlTail(SString tail){m_sSqlTail = tail;}

private:
	//////////////////////////////////////////////////////////////////////////
	// 描    述:  执行一次SQL，并清理所有缓存
	// 作    者:  邵凯田
	// 创建时间:  2016-12-15 13:18
	// 参数说明:  void
	// 返 回 值:  void
	//////////////////////////////////////////////////////////////////////////
	void ExecuteSql();

	SString m_sTableName;//对应的表名
	SString m_sFields;//对应的字段名，逗号分隔，如:col1,col2,col3
	int m_iFirstSqlSoc;//第一条语句的时间，0表示尚无未执行的SQL
	int m_iTimeoutSecs;//超时时间，单位:秒
	SString m_sFullSql;
	SString m_sSqlTail;//insert SQL的尾部字符串，如针对MYSQl数据库，可以在插入主键冲突时改为update操作，并能进行批量insert/update，insert into t_oe_element_state (st_sn,current_val,soc) values (1,0,100),(2,1,101) on duplicate key update current_val=values(current_val),soc=values(soc)
	SLock m_Lock;
};

class SSP_BASE_EXPORT CSsp_BatchDml : public CSsp_DmlHelper
{
public:
	CSsp_BatchDml();
	~CSsp_BatchDml();

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  使用SDatabaseOper初始化
	// 作    者:  邵凯田
	// 创建时间:  2016-12-15 13:14
	// 参数说明:  @pDb为数据库操作类指针
	//         :  @dbType为数据库类型
	//         :  @iTimeoutSec表示超时时间，单位:秒
	//         :  @bAsync为数据库SQL可异步并行执行的SQL前导标记符,true表示可异步,false表示不可异步
	// 返 回 值:  void
	//////////////////////////////////////////////////////////////////////////
	void Init(SDatabaseOper *pDb,eSsp_Database_Type dbType,int iTimeoutSec=5,bool bAsync=true);

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  使用SDatabase初始化
	// 作    者:  邵凯田
	// 创建时间:  2017-8-24 19:14
	// 参数说明:  @pDb为数据库类指针
	//         :  @iTimeoutSec表示超时时间，单位:秒
	//         :  @bAsync为数据库SQL可异步并行执行的SQL前导标记符,true表示可异步,false表示不可异步
	// 返 回 值:  void
	//////////////////////////////////////////////////////////////////////////
	void Init(SDatabase *pDb,int iTimeoutSec=5,bool bAsync=true);


	//////////////////////////////////////////////////////////////////////////
	// 描    述:  添加一条SQL语句到缓存区，如缓存满则立即执行并清空缓存
	// 作    者:  邵凯田
	// 创建时间:  2016-12-15 13:22
	// 参数说明:  @sql为单一SQL语句，一般为delete update
	// 返 回 值:  void
	//////////////////////////////////////////////////////////////////////////
	void AddSql(SString sql);


	//////////////////////////////////////////////////////////////////////////
	// 描    述:  进行一次超时检查，如到达超时时间，立即执行缓存SQL
	// 作    者:  邵凯田
	// 创建时间:  2016-12-15 13:16
	// 参数说明:  @now_soc表示当前时间，0表示不输入时间
	// 返 回 值:  void
	//////////////////////////////////////////////////////////////////////////
	void CheckTimeOut(int now_soc=0);

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  立即执行缓存SQL
	// 作    者:  钱程林
	// 创建时间:  2018-01-19 10:02
	// 参数说明:  void
	// 返 回 值:  void
	//////////////////////////////////////////////////////////////////////////
	void Flush();

private:
	//////////////////////////////////////////////////////////////////////////
	// 描    述:  执行一次SQL，并清理所有缓存
	// 作    者:  邵凯田
	// 创建时间:  2016-12-15 13:18
	// 参数说明:  void
	// 返 回 值:  void
	//////////////////////////////////////////////////////////////////////////
	void ExecuteSql();

	int m_iFirstSqlSoc;//第一条语句的时间，0表示尚无未执行的SQL
	int m_iTimeoutSecs;//超时时间，单位:秒
	SString m_sFullSql;
	SLock m_Lock;
};


#endif//__SSP_DATABASE_CONFIG_H__
