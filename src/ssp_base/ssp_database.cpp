/**
 *
 * 文 件 名 : ssp_database.cpp
 * 创建日期 : 2015-7-29 19:34
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

#include "ssp_database.h"
#include <math.h>

CSsp_DmlHelper::CSsp_DmlHelper()
{
	m_pDbOper = NULL;
	m_pDb = NULL;
}

CSsp_DmlHelper::~CSsp_DmlHelper()
{

}

//////////////////////////////////////////////////////////////////////////
// 描    述:  将SQL语句中日期类型转为字符串类型字段
// 作    者:  邵凯田
// 创建时间:  2016-12-15 16:14
// 参数说明:  @sField为字段名称
// 返 回 值:  根据不同的数据库类型而不同
//////////////////////////////////////////////////////////////////////////
SString CSsp_DmlHelper::Date2String(SString sField)
{
	switch (m_eDbType)
	{
	case DB_ORACLE:
		return "to_char("+sField+",'yyyy-mm-dd hh24:mi:ss')";

	case DB_MYSQL:
		return "date_format("+sField+",'%Y-%m-%d %k:%i:%s')";
	default:
		return sField;
	}
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  将SQL语句中字符串改为日期类型
// 作    者:  邵凯田
// 创建时间:  2016-12-15 16:14
// 参数说明:  @sDateTime格式为:yyyy-MM-dd hh:mm:ss
// 返 回 值:  根据不同的数据库类型而不同
//////////////////////////////////////////////////////////////////////////
SString CSsp_DmlHelper::String2Date(SString sDateTime)
{
	switch (m_eDbType)
	{
	case DB_ORACLE:
		return "to_date('"+sDateTime+"','yyyy-mm-dd hh24:mi:ss')";

	case DB_MYSQL:
		return "str_to_date('"+sDateTime+"','%Y-%m-%d %k:%i:%s')";
	default:
		return "'"+sDateTime+"'";
	}
}



CSsp_Database::CSsp_Database()
{
	SetConfigType(SPCFG_DB_CONFIG);
	SetAutoReload(true);
	m_pHisMasterDbPools = NULL;
	m_pHisSlaveDbPools = NULL;
	m_pMdbDbPools = NULL;
	m_iPoolSize = 3;
	m_MasterDbType = m_SlaveDbType = DB_UNKNOWN;
}

CSsp_Database::~CSsp_Database()
{
	if(m_pHisMasterDbPools != NULL)
		delete m_pHisMasterDbPools;
	if(m_pHisSlaveDbPools != NULL)
		delete m_pHisSlaveDbPools;
	if(m_pMdbDbPools != NULL)
		delete m_pMdbDbPools;
}

#define UKDB_PWD_MASK "iuiqbaio2947ngf2"

//////////////////////////////////////////////////////////////////////////
// 描    述:  加载指定的配置文件
// 作    者:  邵凯田
// 创建时间:  2015-7-29 19:31
// 参数说明:  @sPathFile为数据库配置文件名
// 返 回 值:  true表示加载成功，false表示加载失败
//////////////////////////////////////////////////////////////////////////
bool CSsp_Database::Load(SString sPathFile)
{
	CConfigBase::Load(sPathFile);
	SXmlConfig xml;
	if(!xml.ReadConfig(sPathFile))
		return false;
	bool bDML_HDB_publish=false;//主数据库是否发布DML语句(insert/update/delete)
	bool bDML_MDB_publish=false;//内存数据库是否发布DML语句(insert/update/delete)
	SString hdb_ip,mdb_ip;
	int hdb_port=0,mdb_port=0;
	
	SBaseConfig *pCfg = xml.SearchChild("master");
	if(pCfg)
	{
		m_bMaster = true;
		m_sMasterType		= pCfg->GetAttribute("type");//mysql/pgsql/oracle/dameng/mdb
		m_sMasterHostAddr	= pCfg->GetAttribute("hostaddr");
		m_iMasterPort		= pCfg->GetAttributeI("port");
		m_sMasterUser		= pCfg->GetAttribute("user");
		m_sMasterPassword	= SApi::Decrypt_String(pCfg->GetAttribute("password"),UKDB_PWD_MASK);
		m_sMasterDbName		= pCfg->GetAttribute("dbname");
		bDML_HDB_publish	= pCfg->GetAttribute("dml_publish")=="true";
		if(bDML_HDB_publish)
		{
			hdb_ip			= pCfg->GetAttribute("dml_publish_ip");
			hdb_port		= pCfg->GetAttributeI("dml_publish_port");
		}
	}
	else
	{
		m_bMaster = false;
	}

	pCfg = xml.SearchChild("slave");
	if(pCfg)
	{
		m_bSlave = true;
		m_sSlaveType		= pCfg->GetAttribute("type");//mysql/pgsql/oracle/dameng/mdb
		m_sSlaveHostAddr	= pCfg->GetAttribute("hostaddr");
		m_iSlavePort		= pCfg->GetAttributeI("port");
		m_sSlaveUser		= pCfg->GetAttribute("user");
		m_sSlavePassword	= SApi::Decrypt_String(pCfg->GetAttribute("password"),UKDB_PWD_MASK);
		m_sSlaveDbName		= pCfg->GetAttribute("dbname");
	}
	else
	{
		m_bSlave = false;
	}

	pCfg = xml.SearchChild("mdb");
	if(pCfg)
	{
		m_bMdb = true;
		//m_sMdbType		= pCfg->GetAttribute("type");//mysql/pgsql/oracle/dameng/mdb
		m_sMdbHostAddr		= pCfg->GetAttribute("hostaddr");
		m_iMdbPort			= pCfg->GetAttributeI("port");
		m_sMdbUser			= pCfg->GetAttribute("user");
		m_sMdbPassword		= SApi::Decrypt_String(pCfg->GetAttribute("password"),UKDB_PWD_MASK);
		m_sMdbDbName		= pCfg->GetAttribute("dbname");
		bDML_MDB_publish	= pCfg->GetAttribute("dml_publish")=="true";
		if(bDML_MDB_publish)
		{
			mdb_ip			= pCfg->GetAttribute("dml_publish_ip");
			mdb_port		= pCfg->GetAttributeI("dml_publish_port");
		}
	}
	else
	{
		m_bMdb = false;
	}

	bool bReload = false;
	if(m_pHisMasterDbPools != NULL || m_pMdbDbPools != NULL)
		bReload = true;
	//SDatabasePool<SDatabase> *pOldMaster,*pOldSlave,*pOldMdb;
	SDatabasePool<SDatabase> *pHisMaster,*pHisSlave,*pMdb;
// 	pOldMaster = m_pHisMasterDbPools;
// 	pOldSlave = m_pHisSlaveDbPools;
// 	pOldMdb = m_pMdbDbPools;
	pHisMaster =  pHisSlave = pMdb = NULL;
	if(m_bMaster == false)
	{
		LOGFAULT("主数据库未配置!");
		goto err;
	}
	if(m_sMasterType.toLower() == "mdb")
	{
		m_MasterDbType = DB_MDB;
		SDatabasePool<SMdb> *pNewPool = new SDatabasePool<SMdb>;
		pHisMaster = (SDatabasePool<SDatabase> *) pNewPool;
		pNewPool->SetParams(GetMasterConnectString());
		if(!pNewPool->CreateDatabasePool(m_iPoolSize))
		{
			delete pNewPool;
			pHisMaster = NULL;
			LOGERROR("创建MDB主数据库连接池时失败");
			goto err;
		}
	}
#ifdef SSP_DBUSED_MYSQL
	else if(m_sMasterType.toLower() == "mysql")
	{
		m_MasterDbType = DB_MYSQL;
		SDatabasePool<SMySQL> *pNewPool = new SDatabasePool<SMySQL>;
		pHisMaster = (SDatabasePool<SDatabase> *) pNewPool;
		pNewPool->SetParams(GetMasterConnectString());
		if(!pNewPool->CreateDatabasePool(m_iPoolSize))
		{
			delete pNewPool;
			pHisMaster = NULL;
			LOGERROR("创建MYSQL主数据库连接池时失败");
			goto err;
		}
	}
#endif

#ifdef SSP_DBUSED_ODBC
	else if(m_sMasterType.toLower() == "odbc")
	{
		m_MasterDbType = DB_ODBC;
		SDatabasePool<SOdbc> *pNewPool = new SDatabasePool<SOdbc>;
		pHisMaster = (SDatabasePool<SDatabase> *) pNewPool;
		pNewPool->SetParams(GetMasterConnectString());
		if(!pNewPool->CreateDatabasePool(m_iPoolSize))
		{
			delete pNewPool;
			pHisMaster = NULL;
			LOGERROR("创建ODBC主数据库连接池时失败");
			goto err;
		}
	}
#endif

#ifdef SSP_DBUSED_ORACLE
	else if(m_sMasterType.toLower() == "oracle")
	{
		m_MasterDbType = DB_ORACLE;
		SDatabasePool<SOracle> *pNewPool = new SDatabasePool<SOracle>;
		pHisMaster = (SDatabasePool<SDatabase> *) pNewPool;
		pNewPool->SetParams(GetMasterConnectString());
		if(!pNewPool->CreateDatabasePool(m_iPoolSize))
		{
			delete pNewPool;
			pHisMaster = NULL;
			LOGERROR("创建ORACLE主数据库连接池时失败");
			goto err;
		}
	}
#endif
	/*
	else if(m_sMasterType.toLower() == "pgsql")
		pHisMaster = (SDatabasePool<SDatabase> *)new SDatabasePool<SPostgres>;
	else if(m_sMasterType.toLower() == "oracle")
		pHisMaster = (SDatabasePool<SDatabase> *)new SDatabasePool<SOracle>;
	else if(m_sMasterType.toLower() == "dameng")
		pHisMaster = (SDatabasePool<SDatabase> *)new SDatabasePool<SDameng>;
	*/
	else
	{
		LOGFAULT("未知的主数据库类型:%s",m_sMasterType.data());
		goto err;
	}	

	if(m_bSlave)
	{
		if(m_sSlaveType.toLower() == "mdb")
		{
			m_SlaveDbType = DB_MDB;
			SDatabasePool<SMdb> *pNewPool = new SDatabasePool<SMdb>;
			pHisSlave = (SDatabasePool<SDatabase> *) pNewPool;
			pNewPool->SetParams(GetMasterConnectString());
			if(!pNewPool->CreateDatabasePool(m_iPoolSize))
			{
				delete pNewPool;
				pHisSlave = NULL;
				LOGERROR("创建MDB备数据库连接池时失败");
				goto err;
			}
		}
#ifdef SSP_DBUSED_MYSQL
		else if(m_sSlaveType.toLower() == "mysql")
		{
			m_SlaveDbType = DB_MYSQL;
			SDatabasePool<SMySQL> *pNewPool = new SDatabasePool<SMySQL>;
			pHisSlave = (SDatabasePool<SDatabase> *) pNewPool;
			pNewPool->SetParams(GetMasterConnectString());
			if(!pNewPool->CreateDatabasePool(m_iPoolSize))
			{
				delete pNewPool;
				pHisSlave = NULL;
				LOGERROR("创建MYSQL备数据库连接池时失败");
				goto err;
			}
		}
#endif

#ifdef SSP_DBUSED_ODBC
		else if(m_sSlaveType.toLower() == "odbc")
		{
			m_SlaveDbType = DB_ODBC;
			SDatabasePool<SOdbc> *pNewPool = new SDatabasePool<SOdbc>;
			pHisSlave = (SDatabasePool<SDatabase> *) pNewPool;
			pNewPool->SetParams(GetMasterConnectString());
			if(!pNewPool->CreateDatabasePool(m_iPoolSize))
			{
				delete pNewPool;
				pHisSlave = NULL;
				LOGERROR("创建ODBC备数据库连接池时失败");
				goto err;
			}
		}
#endif

#ifdef SSP_DBUSED_ORACLE
		else if(m_sSlaveType.toLower() == "oracle")
		{
			m_SlaveDbType = DB_ORACLE;
			SDatabasePool<SOracle> *pNewPool = new SDatabasePool<SOracle>;
			pHisSlave = (SDatabasePool<SDatabase> *) pNewPool;
			pNewPool->SetParams(GetMasterConnectString());
			if(!pNewPool->CreateDatabasePool(m_iPoolSize))
			{
				delete pNewPool;
				pHisSlave = NULL;
				LOGERROR("创建ORACLE备数据库连接池时失败");
				goto err;
			}
		}
#endif
		/*
		else if(m_sSlaveType.toLower() == "pgsql")
			pHisSlave = (SDatabasePool<SDatabase> *)new SDatabasePool<SPostgres>;
		else if(m_sSlaveType.toLower() == "oracle")
			pHisSlave = (SDatabasePool<SDatabase> *)new SDatabasePool<SOracle>;
		else if(m_sSlaveType.toLower() == "dameng")
			pHisSlave = (SDatabasePool<SDatabase> *)new SDatabasePool<SDameng>;
		*/
		else
		{
			LOGFAULT("未知的备数据库类型:%s",m_sSlaveType.data());
			goto err;
		}
	}

	if(m_bMdb)
	{
		SDatabasePool<SMdb> *pNewPool = new SDatabasePool<SMdb>;
		pMdb = (SDatabasePool<SDatabase> *)pNewPool;		
		pNewPool->SetParams(GetMdbConnectString());
		if(!pNewPool->CreateDatabasePool(m_iPoolSize))
		{
			delete pNewPool;
			pMdb = NULL;
			LOGERROR("创建内存数据库连接池时失败");
			goto err;
		}
	}
	//m_HisDbOper.Quit();
	m_HisDbOper.SetDatabasePool(pHisMaster);
	m_HisDbOper.SetSlaveDatabasePool(pHisSlave);
	if(pHisSlave != NULL)
		m_HisDbOper.Start();
	if(bDML_HDB_publish)
		m_HisDbOper.StartDmlPublish(hdb_ip,hdb_port);
	m_MdbOper.SetDatabasePool(pMdb);
	if(bDML_MDB_publish)
		m_MdbOper.StartDmlPublish(mdb_ip,mdb_port);
	if(bReload)
	{
		m_pOldHisMasterDbPools	= m_pHisMasterDbPools;
		m_pOldHisSlaveDbPools	= m_pHisSlaveDbPools;
		m_pOldMdbDbPools		= m_pMdbDbPools;
		S_CREATE_THREAD(ThreadDelayFree,this);
	}
	m_pHisMasterDbPools = pHisMaster;
	m_pHisSlaveDbPools = pHisSlave;
	m_pMdbDbPools = pMdb;

	return true;

err:
	if(pHisMaster != NULL)
		delete pHisMaster;
	if(pHisSlave != NULL)
		delete pHisSlave;
	if(pMdb != NULL)
		delete pMdb;
	return false;
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  连接池资源的延时释放类
// 作    者:  邵凯田
// 创建时间:  2015-7-30 14:05
// 参数说明:  @lp为this
// 返 回 值:  NULL
//////////////////////////////////////////////////////////////////////////
void* CSsp_Database::ThreadDelayFree(void *lp)
{
	CSsp_Database *pThis = ( CSsp_Database *)lp;
	S_INTO_THREAD;
	SApi::UsSleep(5000000);//5秒后释放

	if(pThis->m_pOldHisMasterDbPools != NULL)
	{
		delete pThis->m_pOldHisMasterDbPools;
		pThis->m_pOldHisMasterDbPools = NULL;
	}
	if(pThis->m_pOldHisSlaveDbPools	!= NULL)
	{
		delete pThis->m_pOldHisSlaveDbPools;
		pThis->m_pOldHisSlaveDbPools = NULL;
	}
	if(pThis->m_pOldMdbDbPools != NULL)
	{
		delete pThis->m_pOldMdbDbPools;
		pThis->m_pOldMdbDbPools = NULL;
	}
	return NULL;
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  取主、备、内存数据库的连接字符串
// 作    者:  邵凯田
// 创建时间:  2015-7-29 20:04
// 参数说明:  void
// 返 回 值:  SString,用于SDatabase实例的连接参数
//////////////////////////////////////////////////////////////////////////
SString CSsp_Database::GetMasterConnectString()
	{
	return SString::toFormat("hostaddr=%s;port=%d;user=%s;password=%s;dbname=%s;",
		m_sMasterHostAddr.data(),
		m_iMasterPort,
		m_sMasterUser.data(),
		m_sMasterPassword.data(),
		m_sMasterDbName.data());
}

SString CSsp_Database::GetSlaveConnectString()
{
	return SString::toFormat("hostaddr=%s;port=%d;user=%s;password=%s;dbname=%s;",
		m_sSlaveHostAddr.data(),
		m_iSlavePort,
		m_sSlaveUser.data(),
		m_sSlavePassword.data(),
		m_sSlaveDbName.data());
}

SString CSsp_Database::GetMdbConnectString()
{
	return SString::toFormat("hostaddr=%s;port=%d;user=%s;password=%s;dbname=%s;",
		m_sMdbHostAddr.data(),
		m_iMdbPort,
		m_sMdbUser.data(),
		m_sMdbPassword.data(),
		m_sMdbDbName.data());
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  清除所有MDB的内存触发器
// 作    者:  邵凯田
// 创建时间:  2016-5-11 16:49
// 参数说明:  void
// 返 回 值:  void
//////////////////////////////////////////////////////////////////////////
void CSsp_Database::RemoveAllMdbTrigger()
{
	if(m_pMdbDbPools == NULL)
		return;
	for(int i=0;i<m_pMdbDbPools->GetPoolSize();i++)
	{
		SMdb *pDb = (SMdb*)m_pMdbDbPools->GetDatabaseByIdx(i);
		if(pDb != NULL)
			pDb->GetMdbClient()->RemoveAllTrigger();
	}
}


//////////////////////////////////////////////////////////////////////////
// 名    称:  CSsp_BatchDmlInsert
// 作    者:  邵凯田
// 创建时间:  2016-12-15 10:51
// 描    述:  批量插入的
//////////////////////////////////////////////////////////////////////////
CSsp_BatchDmlInsert::CSsp_BatchDmlInsert()
{
}

CSsp_BatchDmlInsert::~CSsp_BatchDmlInsert()
{

}

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
void CSsp_BatchDmlInsert::Init(SDatabaseOper *pDb,eSsp_Database_Type dbType,char* sTableName,char* sFields,int iTimeoutSec/*=5*/,bool bAsync/*=true*/)
{
	m_pDbOper = pDb;
	m_eDbType = dbType;
	m_bSqlAsyncRun = bAsync;
	m_sTableName = sTableName;
	m_sFields = sFields;
	m_iTimeoutSecs = iTimeoutSec;
	m_iFirstSqlSoc = 0;
	m_sFullSql = "";
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  使用SDatabase初始化
// 作    者:  邵凯田
// 创建时间:  2017-8-24 19:14
// 参数说明:  @pDb为数据库类指针
//         :  @iTimeoutSec表示超时时间，单位:秒
//         :  @bAsync为数据库SQL可异步并行执行的SQL前导标记符,true表示可异步,false表示不可异步
// 返 回 值:  void
//////////////////////////////////////////////////////////////////////////
void CSsp_BatchDmlInsert::Init(SDatabase *pDb,char* sTableName,char* sFields,int iTimeoutSec/*=5*/,bool bAsync/*=true*/)
{
	m_pDb = pDb;
	m_eDbType = pDb->GetDbType();
	m_bSqlAsyncRun = bAsync;
	m_sTableName = sTableName;
	m_sFields = sFields;
	m_iTimeoutSecs = iTimeoutSec;
	m_iFirstSqlSoc = 0;
	m_sFullSql = "";
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  添加一条的值内容，如缓存大小到达，立即执行所有缓存SQL
// 作    者:  邵凯田
// 创建时间:  2016-12-15 13:15
// 参数说明:  @sValues,为对应所有字段的值，逗号分隔，如:1,2,'xx'
// 返 回 值:  void
//////////////////////////////////////////////////////////////////////////
void CSsp_BatchDmlInsert::AddInsertValues(SString sValues)
{
	m_Lock.lock();
	if(m_sFullSql.length() == 0)
	{
		switch(m_eDbType)
		{
		case DB_MDB:
			break;
		case DB_ORACLE:
			m_sFullSql = "insert all ";
			break;
		case DB_MYSQL:
			m_sFullSql = "insert into "+m_sTableName+" ("+m_sFields+") values ";
			break;
		default:
			m_sFullSql = "insert into "+m_sTableName+" ("+m_sFields+") ";
			break;
		}
	}

	switch(m_eDbType)
	{
	case DB_MDB:
		if(m_sFullSql.length() > 0)
			m_sFullSql += "{$SQL_SEP$}";
		m_sFullSql = "insert into "+m_sTableName+" ("+m_sFields+") values ("+sValues+") ";
		break;
	case DB_ORACLE:
		m_sFullSql += "into "+m_sTableName+" ("+m_sFields+") values ("+sValues+") ";
		break;
	case DB_MYSQL:
		if(m_iFirstSqlSoc != 0)
			m_sFullSql += ",";
		m_sFullSql +="("+sValues+") ";
		break;
	default:
		m_sFullSql +="values ("+sValues+") ";
		ExecuteSql();
		break;
	}
	if(m_sFullSql.length() >= C_BATCH_DML_MAX_BUFFERSIZE)
		ExecuteSql();
	else if(m_iFirstSqlSoc == 0)
		m_iFirstSqlSoc = (int)SDateTime::getNowSoc();
	m_Lock.unlock();
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  进行一次超时检查，如到达超时时间，立即执行缓存SQL
// 作    者:  邵凯田
// 创建时间:  2016-12-15 13:16
// 参数说明:  @now_soc表示当前时间，0表示不输入时间
// 返 回 值:  void
//////////////////////////////////////////////////////////////////////////
void CSsp_BatchDmlInsert::CheckTimeOut(int now_soc/*=0*/)
{
	if(m_iFirstSqlSoc == 0)
		return;
	m_Lock.lock();
	if(m_iFirstSqlSoc == 0)
	{
		m_Lock.unlock();
		return;
	}
	if(now_soc == 0)
		now_soc = (int)SDateTime::getNowSoc();
	if(abs(now_soc - m_iFirstSqlSoc) >= m_iTimeoutSecs)
		ExecuteSql();
	m_Lock.unlock();
}

void CSsp_BatchDmlInsert::Flush()
{
	m_Lock.lock();
	ExecuteSql();
	m_Lock.unlock();
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  执行一次SQL，并清理所有缓存
// 作    者:  邵凯田
// 创建时间:  2016-12-15 13:18
// 参数说明:  void
// 返 回 值:  void
//////////////////////////////////////////////////////////////////////////
void CSsp_BatchDmlInsert::ExecuteSql()
{
	if(m_bSqlAsyncRun)
		m_sFullSql = C_SQL_ASYNC_RUN+m_sFullSql;
	switch(m_eDbType)
	{
	case DB_ORACLE:
		m_sFullSql += "select 1 from dual";
		break;
	default:
		break;
	}
	m_sFullSql += m_sSqlTail;
	if(m_pDbOper != NULL)
		m_pDbOper->Execute(m_sFullSql);
	else
		m_pDb->Execute(m_sFullSql);
	m_iFirstSqlSoc = 0;
	m_sFullSql = "";
}


CSsp_BatchDml::CSsp_BatchDml()
{

}
CSsp_BatchDml::~CSsp_BatchDml()
{

}

//////////////////////////////////////////////////////////////////////////
// 描    述:  初始化
// 作    者:  邵凯田
// 创建时间:  2016-12-15 13:14
// 参数说明:  @pDb为数据库操作类指针
//         :  @dbType为数据库类型
//         :  @iTimeoutSec表示超时时间，单位:秒
//         :  @bAsync为数据库SQL可异步并行执行的SQL前导标记符,true表示可异步,false表示不可异步
// 返 回 值:  void
//////////////////////////////////////////////////////////////////////////
void CSsp_BatchDml::Init(SDatabaseOper *pDb,eSsp_Database_Type dbType,int iTimeoutSec/*=5*/,bool bAsync/*=true*/)
{
	m_pDbOper = pDb;
	m_eDbType = dbType;
	m_bSqlAsyncRun = bAsync;
	m_iTimeoutSecs = iTimeoutSec;
	m_sFullSql = "";
	m_iFirstSqlSoc = 0;
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  使用SDatabase初始化
// 作    者:  邵凯田
// 创建时间:  2017-8-24 19:14
// 参数说明:  @pDb为数据库类指针
//         :  @iTimeoutSec表示超时时间，单位:秒
//         :  @bAsync为数据库SQL可异步并行执行的SQL前导标记符,true表示可异步,false表示不可异步
// 返 回 值:  void
//////////////////////////////////////////////////////////////////////////
void CSsp_BatchDml::Init(SDatabase *pDb,int iTimeoutSec/*=5*/,bool bAsync/*=true*/)
{
	m_pDb = pDb;
	m_eDbType = pDb->GetDbType();
	m_bSqlAsyncRun = bAsync;
	m_iTimeoutSecs = iTimeoutSec;
	m_sFullSql = "";
	m_iFirstSqlSoc = 0;
}


//////////////////////////////////////////////////////////////////////////
// 描    述:  添加一条SQL语句到缓存区，如缓存满则立即执行并清空缓存
// 作    者:  邵凯田
// 创建时间:  2016-12-15 13:22
// 参数说明:  @sql为单一SQL语句，一般为delete update
// 返 回 值:  void
//////////////////////////////////////////////////////////////////////////
void CSsp_BatchDml::AddSql(SString sql)
{
	m_Lock.lock();
	switch(m_eDbType)
	{
	case DB_ORACLE:
		if(m_sFullSql.length() == 0)
			m_sFullSql = "begin ";
		m_sFullSql += sql + ";commit;";
		if(m_sFullSql.length() >= C_BATCH_DML_MAX_BUFFERSIZE)
			ExecuteSql();
		else if(m_iFirstSqlSoc == 0)
			m_iFirstSqlSoc = (int)SDateTime::getNowSoc();
		break;
	case DB_MDB:
		if(m_sFullSql.length() > 0)
			m_sFullSql += "{$SQL_SEP$}";
		m_sFullSql += sql;
		if(m_sFullSql.length() >= C_BATCH_DML_MAX_BUFFERSIZE)
			ExecuteSql();
		else if(m_iFirstSqlSoc == 0)
			m_iFirstSqlSoc = (int)SDateTime::getNowSoc();
		break;
	default:
		m_sFullSql = sql;
		ExecuteSql();
		break;
	}
	m_Lock.unlock();
}


//////////////////////////////////////////////////////////////////////////
// 描    述:  进行一次超时检查，如到达超时时间，立即执行缓存SQL
// 作    者:  邵凯田
// 创建时间:  2016-12-15 13:16
// 参数说明:  @now_soc表示当前时间，0表示不输入时间
// 返 回 值:  void
//////////////////////////////////////////////////////////////////////////
void CSsp_BatchDml::CheckTimeOut(int now_soc/*=0*/)
{
	if(m_iFirstSqlSoc == 0)
		return;
	m_Lock.lock();
	if(m_iFirstSqlSoc == 0)
	{
		m_Lock.unlock();
		return;
	}
	if(now_soc == 0)
		now_soc = (int)SDateTime::getNowSoc();
	if(abs(now_soc - m_iFirstSqlSoc) >= m_iTimeoutSecs)
		ExecuteSql();
	m_Lock.unlock();
}

void CSsp_BatchDml::Flush()
{
	m_Lock.lock();
	ExecuteSql();
	m_Lock.unlock();
}

//////////////////////////////////////////////////////////////////////////
// 描    述:  执行一次SQL，并清理所有缓存
// 作    者:  邵凯田
// 创建时间:  2016-12-15 13:18
// 参数说明:  void
// 返 回 值:  void
//////////////////////////////////////////////////////////////////////////
void CSsp_BatchDml::ExecuteSql()
{
	switch(m_eDbType)
	{
	case DB_ORACLE:
		m_sFullSql += " end;";
		break;
	default:
		break;
	}
	if(m_bSqlAsyncRun)
		m_sFullSql = C_SQL_ASYNC_RUN+m_sFullSql;
	if(m_pDbOper != NULL)
		m_pDbOper->Execute(m_sFullSql);
	else
		m_pDb->Execute(m_sFullSql);
	switch(m_eDbType)
	{
	case DB_ORACLE:
		m_sFullSql = "begin ";
		break;
	default:
		m_sFullSql = "";
		break;
	}
	m_iFirstSqlSoc = 0;
}

