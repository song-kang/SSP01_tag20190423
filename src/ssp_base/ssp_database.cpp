/**
 *
 * �� �� �� : ssp_database.cpp
 * �������� : 2015-7-29 19:34
 * ��    �� : �ۿ���(skt001@163.com)
 * �޸����� : $Date: $
 * ��ǰ�汾 : $Revision: $
 * �������� : ���ݿ�������
 * �޸ļ�¼ : 
 *            $Log: $
 *
 * Ver  Date        Author  Comments
 * ---  ----------  ------  -------------------------------------------
 * 001	2015-7-29	�ۿ�������ļ�
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
// ��    ��:  ��SQL�������������תΪ�ַ��������ֶ�
// ��    ��:  �ۿ���
// ����ʱ��:  2016-12-15 16:14
// ����˵��:  @sFieldΪ�ֶ�����
// �� �� ֵ:  ���ݲ�ͬ�����ݿ����Ͷ���ͬ
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
// ��    ��:  ��SQL������ַ�����Ϊ��������
// ��    ��:  �ۿ���
// ����ʱ��:  2016-12-15 16:14
// ����˵��:  @sDateTime��ʽΪ:yyyy-MM-dd hh:mm:ss
// �� �� ֵ:  ���ݲ�ͬ�����ݿ����Ͷ���ͬ
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

//#define UKDB_PWD_MASK "iuiqbaio2947ngf2"
#define UKDB_PWD_MASK "yqqlm^gsycl.1978"
//////////////////////////////////////////////////////////////////////////
// ��    ��:  ����ָ���������ļ�
// ��    ��:  �ۿ���
// ����ʱ��:  2015-7-29 19:31
// ����˵��:  @sPathFileΪ���ݿ������ļ���
// �� �� ֵ:  true��ʾ���سɹ���false��ʾ����ʧ��
//////////////////////////////////////////////////////////////////////////
bool CSsp_Database::Load(SString sPathFile)
{
	CConfigBase::Load(sPathFile);
	SXmlConfig xml;
	if(!xml.ReadConfig(sPathFile))
		return false;
	bool bDML_HDB_publish=false;//�����ݿ��Ƿ񷢲�DML���(insert/update/delete)
	bool bDML_MDB_publish=false;//�ڴ����ݿ��Ƿ񷢲�DML���(insert/update/delete)
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
		LOGFAULT("�����ݿ�δ����!");
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
			LOGERROR("����MDB�����ݿ����ӳ�ʱʧ��");
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
			LOGERROR("����MYSQL�����ݿ����ӳ�ʱʧ��");
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
			LOGERROR("����ODBC�����ݿ����ӳ�ʱʧ��");
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
			LOGERROR("����ORACLE�����ݿ����ӳ�ʱʧ��");
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
		LOGFAULT("δ֪�������ݿ�����:%s",m_sMasterType.data());
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
				LOGERROR("����MDB�����ݿ����ӳ�ʱʧ��");
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
				LOGERROR("����MYSQL�����ݿ����ӳ�ʱʧ��");
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
				LOGERROR("����ODBC�����ݿ����ӳ�ʱʧ��");
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
				LOGERROR("����ORACLE�����ݿ����ӳ�ʱʧ��");
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
			LOGFAULT("δ֪�ı����ݿ�����:%s",m_sSlaveType.data());
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
			LOGERROR("�����ڴ����ݿ����ӳ�ʱʧ��");
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
// ��    ��:  ���ӳ���Դ����ʱ�ͷ���
// ��    ��:  �ۿ���
// ����ʱ��:  2015-7-30 14:05
// ����˵��:  @lpΪthis
// �� �� ֵ:  NULL
//////////////////////////////////////////////////////////////////////////
void* CSsp_Database::ThreadDelayFree(void *lp)
{
	CSsp_Database *pThis = ( CSsp_Database *)lp;
	S_INTO_THREAD;
	SApi::UsSleep(5000000);//5����ͷ�

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
// ��    ��:  ȡ���������ڴ����ݿ�������ַ���
// ��    ��:  �ۿ���
// ����ʱ��:  2015-7-29 20:04
// ����˵��:  void
// �� �� ֵ:  SString,����SDatabaseʵ�������Ӳ���
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
// ��    ��:  �������MDB���ڴ津����
// ��    ��:  �ۿ���
// ����ʱ��:  2016-5-11 16:49
// ����˵��:  void
// �� �� ֵ:  void
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
// ��    ��:  CSsp_BatchDmlInsert
// ��    ��:  �ۿ���
// ����ʱ��:  2016-12-15 10:51
// ��    ��:  ���������
//////////////////////////////////////////////////////////////////////////
CSsp_BatchDmlInsert::CSsp_BatchDmlInsert()
{
}

CSsp_BatchDmlInsert::~CSsp_BatchDmlInsert()
{

}

//////////////////////////////////////////////////////////////////////////
// ��    ��:  ��ʼ��
// ��    ��:  �ۿ���
// ����ʱ��:  2016-12-15 13:14
// ����˵��:  @pDbΪ���ݿ������ָ��
//         :  @dbTypeΪ���ݿ�����
//         :  @sTableName��ʾ������
//         :  @sFields��ʾ�ֶ��������ŷָ�����:col1,col2,col3
//         :  @iTimeoutSec��ʾ��ʱʱ�䣬��λ:��
//         :  @bAsyncΪ���ݿ�SQL���첽����ִ�е�SQLǰ����Ƿ�,true��ʾ���첽,false��ʾ�����첽
// �� �� ֵ:  void
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
// ��    ��:  ʹ��SDatabase��ʼ��
// ��    ��:  �ۿ���
// ����ʱ��:  2017-8-24 19:14
// ����˵��:  @pDbΪ���ݿ���ָ��
//         :  @iTimeoutSec��ʾ��ʱʱ�䣬��λ:��
//         :  @bAsyncΪ���ݿ�SQL���첽����ִ�е�SQLǰ����Ƿ�,true��ʾ���첽,false��ʾ�����첽
// �� �� ֵ:  void
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
// ��    ��:  ���һ����ֵ���ݣ��绺���С�������ִ�����л���SQL
// ��    ��:  �ۿ���
// ����ʱ��:  2016-12-15 13:15
// ����˵��:  @sValues,Ϊ��Ӧ�����ֶε�ֵ�����ŷָ�����:1,2,'xx'
// �� �� ֵ:  void
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
// ��    ��:  ����һ�γ�ʱ��飬�絽�ﳬʱʱ�䣬����ִ�л���SQL
// ��    ��:  �ۿ���
// ����ʱ��:  2016-12-15 13:16
// ����˵��:  @now_soc��ʾ��ǰʱ�䣬0��ʾ������ʱ��
// �� �� ֵ:  void
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
// ��    ��:  ִ��һ��SQL�����������л���
// ��    ��:  �ۿ���
// ����ʱ��:  2016-12-15 13:18
// ����˵��:  void
// �� �� ֵ:  void
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
// ��    ��:  ��ʼ��
// ��    ��:  �ۿ���
// ����ʱ��:  2016-12-15 13:14
// ����˵��:  @pDbΪ���ݿ������ָ��
//         :  @dbTypeΪ���ݿ�����
//         :  @iTimeoutSec��ʾ��ʱʱ�䣬��λ:��
//         :  @bAsyncΪ���ݿ�SQL���첽����ִ�е�SQLǰ����Ƿ�,true��ʾ���첽,false��ʾ�����첽
// �� �� ֵ:  void
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
// ��    ��:  ʹ��SDatabase��ʼ��
// ��    ��:  �ۿ���
// ����ʱ��:  2017-8-24 19:14
// ����˵��:  @pDbΪ���ݿ���ָ��
//         :  @iTimeoutSec��ʾ��ʱʱ�䣬��λ:��
//         :  @bAsyncΪ���ݿ�SQL���첽����ִ�е�SQLǰ����Ƿ�,true��ʾ���첽,false��ʾ�����첽
// �� �� ֵ:  void
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
// ��    ��:  ���һ��SQL��䵽���������绺����������ִ�в���ջ���
// ��    ��:  �ۿ���
// ����ʱ��:  2016-12-15 13:22
// ����˵��:  @sqlΪ��һSQL��䣬һ��Ϊdelete update
// �� �� ֵ:  void
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
// ��    ��:  ����һ�γ�ʱ��飬�絽�ﳬʱʱ�䣬����ִ�л���SQL
// ��    ��:  �ۿ���
// ����ʱ��:  2016-12-15 13:16
// ����˵��:  @now_soc��ʾ��ǰʱ�䣬0��ʾ������ʱ��
// �� �� ֵ:  void
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
// ��    ��:  ִ��һ��SQL�����������л���
// ��    ��:  �ۿ���
// ����ʱ��:  2016-12-15 13:18
// ����˵��:  void
// �� �� ֵ:  void
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

