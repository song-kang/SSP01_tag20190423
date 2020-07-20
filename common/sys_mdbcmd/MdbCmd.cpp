/**
*
* 文 件 名 : MdbCmd.cpp
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

#include "MdbCmd.h"

CMdbCmd::CMdbCmd()
{
}

CMdbCmd::~CMdbCmd()
{

}
SString TipAndInput(SString str,SString def)
{
	printf(str.data());
	if (def.length() > 0)
		printf("(缺省:%s):", def.data());
	char buf[10240];
	memset(buf, 0, sizeof(buf));
#ifndef WIN32
	gets(buf);
#else
	gets_s(buf, sizeof(buf));
#endif
	SString ret = buf;
	if (ret == "")
	{
		ret = def;
	}
	return ret;
}
bool CMdbCmd::Start()
{
	//SKT_CREATE_THREAD(ThreadListen, this);
	SString cmd, sRet;
	again:
	{
		SString ip = TipAndInput("请输入数据库IP:", "127.0.0.1");
		int port = TipAndInput("请输入数据库端口:", "1400").toInt();
		SString sParam = SString::toFormat("hostaddr=%s;port=%d;user=skt_mdb;password=skt001@163.com;", ip.data(), port);
		m_mdb.SetParams(sParam.data());
	}
	if (!m_mdb.Connect())
	{
		printf("连接数据库失败!\n");
		goto again;
	}
	else
	{
		printf("连接数据库成功!\n");
	}
	cmd = "help";
	cmd = ProcessTelnetCmd(&m_mdb, cmd);
	printf(cmd.data());
	SApi::UsSleep(10000);
	cmd = "";
	while (1)
	{
		cmd = TipAndInput("[s-mdb]#", "");
		if (cmd == "quit")
			break;
		if (cmd == "")
		{
			//printf("\n");
			continue;
		}
		cmd = ProcessTelnetCmd(&m_mdb, cmd);
		printf(cmd.data());
	}

	return true;
}

bool CMdbCmd::Stop()
{
	return true;
}

BYTE* CMdbCmd::OnMyMdbClientTrgCallback(void* cbParam, SString &sTable, eMdbTriggerType eType, int iTrgRows, int iRowSize, BYTE *pTrgData)
{
	CMdbCmd *pThis = (CMdbCmd*)cbParam;
	if (eType <=0 || eType >= 5)
	{
		printf("Trigger callback with invalid type:%d\n",eType);
		return NULL;
	}
	static char* trg_name[] = { "", "Insert", "Update", "Delete", "Truncate" };
	printf("\r\n[TRG] %s [%s %s Trigger] TrgRows=%d,RowSize=%d\r\n",
			SDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz").data(),
			sTable.data(),trg_name[(int)eType],iTrgRows,iRowSize);
	return NULL;
}

extern char g_ver[32];
SString CMdbCmd::ProcessTelnetCmd(SMdb *pMdb, SString &cmd)
{
	SString sRet = "";
	if (cmd == "welcome")
	{
		sRet += "欢迎使用MDB数据库终端工具\r\n版本号:V" + SString::toString((char*)g_ver, strlen(g_ver)) + "\r\n登录时间:" + SDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") + "\r\n";
		sRet += "请输入'help'查看操作命令详情!\r\n";
	}
	else if (cmd == "help")
	{		
		sRet += "--------------------------------------------------------------\r\n";
		sRet += "MDB数据库终端工具(V" + SString::toString((char*)g_ver,strlen(g_ver)) + ")帮助信息\r\n";
		sRet += "作者:邵凯田(skt001@163.com)\r\n";
		sRet += "--------------------------------------------------------------\r\n";
		sRet += "help                 显示帮助信息\n";
		sRet += "trg_on <table_name>  连接指定表的触发器\n";
		sRet += "trg_off <table_name> 断开指定表的触发器\n";
		sRet += "run <sql_file>       执行SQL文件\n";
		sRet += "blob_put             更新大字段\n";
		sRet += "blob_get             读取大字段\n";
		sRet += "quit                 退出\n";
		sRet += "<SQL>                SQL结构化查询语言（仅支持简化语法）\n";
	}
	else if (cmd.find("trg_on") >= 0)
	{
		SString sTableName = SString::GetIdAttribute(2, cmd, " ");
		if (sTableName.length() == 0)
		{
			sRet += "请输入正确的表名!";
			return sRet;
		}
		//pMdb->GetMdbClient()->RemoveTriggerCallback(OnMyMdbClientTrgCallback, this, sTableName,0x0f);
		pMdb->GetMdbClient()->RegisterTriggerCallback(OnMyMdbClientTrgCallback, this, sTableName, 0x0f);
	}
	else if (cmd.find("trg_off") >= 0)
	{
		SString sTableName = SString::GetIdAttribute(2, cmd, " ");
		if (sTableName.length() == 0)
		{
			sRet += "请输入正确的表名!";
			return sRet;
		}
		pMdb->GetMdbClient()->RemoveTriggerCallback(OnMyMdbClientTrgCallback, this, sTableName, 0x0f);
	}
	else if (cmd.find("blob_put") >= 0)
	{
		SString table = TipAndInput("请输入表名:", "");
		SString field = TipAndInput("请输入字段名:", "");
		SString where = TipAndInput("请输入条件:", "");
		SString file = TipAndInput("请输入待读取的本地文件全路径名:", "");
		int ret = pMdb->UpdateLobFromFile(table, field, where, file);
		printf("更新大字段返回:%d\n", ret);
	}
	else if (cmd.find("blob_get") >= 0)
	{
		SString table = TipAndInput("请输入表名:", "");
		SString field = TipAndInput("请输入字段名:", "");
		SString where = TipAndInput("请输入条件:", "");
		SString file = TipAndInput("请输入待保存的本地文件全路径名:", "");
		int ret = pMdb->ReadLobToFile(table, field, where, file);
		printf("读取大字段返回:%d\n", ret);
	}
	else if (cmd.find("run") == 0)
	{
		SString file = cmd.mid(4);
		if (file.length() == 0)
		{
			sRet += "请输入正确的文件名!";
			return sRet;
		}
		RunSqlFile(file);
	}
	else if (cmd == "")
	{
		sRet += "";
	}
	else
	{
		//执行SQL语句
		SString sql = cmd.stripWhiteSpace();
		if (sql.left(6).toLower() == "select")
		{
			//查询语句
			SRecordset rs;
			int soc1, soc2, usec1, usec2;
			SDateTime::getSystemTime(soc1, usec1);
			int ret = pMdb->Retrieve(sql, rs);
			SDateTime::getSystemTime(soc2, usec2);
			if (ret < 0)
			{
				sRet += "SQL执行失败! ";
				sRet += pMdb->GetLastError() + "\r\n";
			}
			else
			{
				sRet += SString::toFormat("查询返回:%d行%d列,查询历时:%f秒\r\n", rs.GetRows(), rs.GetColumns(), (soc2 - soc1) + ((float)usec2 - (float)usec1) / 1000000);
#if 1
				int row, rows = rs.GetRows();
				int col, cols = rs.GetColumns();
				if (cols <= 0)
				{
					return sRet;
				}
				SString text;
				int len;
				int *col_len = new int[cols];
				int rows_chars = 4;
				if (rows >= 10000000)
					rows_chars = 8;
				else if (rows >= 1000000)
					rows_chars = 7;
				else if (rows >= 100000)
					rows_chars = 6;
				else if (rows >= 10000)
					rows_chars = 5;
				for (col = 0; col<cols; col++)
				{
					col_len[col] = rs.GetColumnName(col).length();
				}
				//先扫描全部数据，确定各列数据长度
				for (row = 0; row<rows; row++)
				{
					for (col = 0; col<cols; col++)
					{
						len = rs.GetValue(row, col).length();
						if (len > col_len[col])
							col_len[col] = len;
					}
				}

				//if (rows <= 30)
				{
					//生成表头
					sRet += SString::toFillString("行号", rows_chars, ' ', false)+" ";
					for (col = 0; col<cols; col++)
					{
						sRet += SString::toFillString(rs.GetColumnName(col), col_len[col], ' ', false);
						sRet += " ";
					}
					sRet += "\r\n";
					sRet += SString::toFillString("-", rows_chars, '-', false)+" ";
					for (col = 0; col<cols; col++)
					{
						sRet += SString::toFillString("-", col_len[col], '-', false);
						sRet += " ";
					}
					sRet += "\r\n";
				}
				bool bPause = true;
				for (row = 0; row<rows; row++)
				{
					if (/*rows>30 &&*/ row> 0 && row % 30 == 0)
					{
						printf(sRet.data());
						sRet = "";
						if (bPause)
						{
							printf("按任意健继续显示(q回车退出,c回车全部显示)...\n");
							char ch = getchar();
							if (ch == 'q')
							{
								printf("中止显示后续记录!\n");
								break;
							}
							if (ch == 'c')
								bPause = false;
						}
						//生成表头
						sRet += SString::toFillString("行号", rows_chars, ' ', false) + " ";
						for (col = 0; col<cols; col++)
						{
							sRet += SString::toFillString(rs.GetColumnName(col), col_len[col], ' ', false);
							sRet += " ";
						}
						sRet += "\r\n";
						sRet += SString::toFillString("-", rows_chars, '-', false) + " ";
						for (col = 0; col<cols; col++)
						{
							sRet += SString::toFillString("-", col_len[col], '-', false);
							sRet += " ";
						}
						sRet += "\r\n";
					}
					sRet += SString::toFillString(SString::toString(row+1), rows_chars, ' ', false) + " ";
					for (col = 0; col<cols; col++)
					{
						sRet += SString::toFillString(rs.GetValue(row, col), col_len[col], ' ', false);
						sRet += " ";
					}
					sRet += "\r\n";
				}
				sRet += SString::toFillString("-", rows_chars, '-', false) + " ";
				for (col = 0; col<cols; col++)
				{
					sRet += SString::toFillString("-", col_len[col], '-', false);
					sRet += " ";
				}
				sRet += "\r\n";
				sRet += SString::toFormat("合计:%d\r\n", rows);
				delete[] col_len;
#endif
			}
		}
		else
		{
			//执行语句
			if (pMdb->Execute(sql))
			{
				sRet += "SQL执行成功!\r\n";
			}
			else
			{
				sRet += "SQL执行失败! ";
				sRet += pMdb->GetLastError()+"\r\n";
			}
		}
	}
	return sRet;
}

void CMdbCmd::RunSqlFile(SString filename)
{
	SFile file(filename);
	if (file.open(IO_ReadOnly) == false)
	{
		printf("文件[%s]不存在！\n", filename.data());
		return;
	}
	char buf[10240];
	int lines = 0;
	SString sLine;
	SString sql = "";
	bool bEnd = false;
	bool bSepClosed = true;
	int pos = 0;
	bool bComment = false;
	SString msg;
	bool bSector = false;
	bool bOk = true;

	while (file.readLine(buf, sizeof(buf)) >= 0)
	{
		sLine = buf;
		if (sLine.length() == 0)
		{
			if (file.atEnd())
				break;
			continue;
		}

		if (sLine.Find("/*") >= 0)
		{
			bComment = true;
		}
		if (bComment == true)
		{
			if (sLine.Find("*/") >= 0)
				bComment = false;
			continue;
		}
		if (sLine.Find("--[BEGIN]") >= 0)
		{
			bSector = true;
			sql = "";
			continue;
		}
		if (bSector == true && sLine.Find("--[END]") >= 0)
		{
			bSector = false;
			//tfile.writeString("\r\nsql="+sql);
			if (!m_mdb.Execute(sql))
			{
				printf("SQL[%s]错误:%s\n", sql.data(), m_mdb.GetLastError().data());
				bOk = false;
				break;
			}
			lines++;
			sql = "";
			bEnd = false;
			continue;
		}
		if (bSector == true)
		{
			sql += sLine + "\r\n";
			continue;
		}
		if (sLine.Find("--") >= 0)
		{
			sLine = sLine.Left(sLine.Find("--"));
		}
		pos = -1;
		while (1)
		{
			if ((pos = sLine.Find("'", pos + 1)) < 0)
				break;
			bSepClosed = !bSepClosed;
		}

		if (bSepClosed && sLine.Find(";") >= 0)
		{
			sLine = sLine.Left(sLine.Find(";"));
			bEnd = true;
		}
		sLine.trim();
		if (bEnd == false && (sLine == "" || sLine.Left(2) == "--" || sLine.Left(1) == "*"))
			continue;
		sql += sLine + " ";
		if (bEnd)
		{
			//tfile.writeString("\r\nsql="+sql);
			if (!m_mdb.Execute(sql))
			{
				printf("SQL[%s]错误:%s\n", sql.data(), m_mdb.GetLastError().data());
				bOk = false;
				break;
			}
			lines++;
			sql = "";
			bEnd = false;
		}
		memset(buf, 0, sizeof(buf));
	}
	if (bOk)
		printf("SQL文件执行完毕!共%d个语句被成功执行!\n", lines);
}