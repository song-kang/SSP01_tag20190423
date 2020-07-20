/**
*
* 文 件 名 : MdbCmd.h
* 创建日期 : 2014-11-28 13:38
* 作    者 : 邵凯田(skt001@163.com)
* 修改日期 : $Date: $
* 当前版本 : $Revision: $
* 功能描述 : MDB数据库控制台客户端
* 修改记录 :
*            $Log: $
*
* Ver  Date        Author  Comments
* ---  ----------  ------  -------------------------------------------
* 001	2014-11-28	邵凯田　创建文件
*
**/

#include "db/mdb/SMdb.h"
#include "SApi.h"
#include "SFile.h"
#include "db/mdb/SMdb.h"
class CMdbCmd
{
public:

	CMdbCmd();
	virtual ~CMdbCmd();

	bool Start();
	bool Stop();

	SString ProcessTelnetCmd(SMdb *pMdb,SString &cmd);
	void RunSqlFile(SString filename);

	static BYTE* OnMyMdbClientTrgCallback(void* cbParam, SString &sTable, eMdbTriggerType eType, int iTrgRows, int iRowSize, BYTE *pTrgData);
	SMdb m_mdb;
};
