#include "dbInteraction.h"


dbInteraction::dbInteraction(void)
{
	curDb = NULL;
	Rs = new mRsignal();
}


dbInteraction::~dbInteraction(void)
{
	delete Rs;
}
int dbInteraction::execSql(SString s_sql)
{
	i_colum =0;
	i_row = 0;
	rs.clear();
	if (curDb==NULL)
	{
		return -1;
	}
	if (manageSql(s_sql))
	{
		return 1;
	}
	int soc1, soc2, usec1, usec2,i_ret;

	if (s_sql.left(6).toLower() == "select")
	{
		Rs->sendSiganl((void *)&s_sql,true);
		SDateTime::getSystemTime(soc1, usec1);
		i_ret = curDb->Retrieve(s_sql,rs);
		SDateTime::getSystemTime(soc2, usec2);
		if (i_ret<0)
		{
			sRet = SString("查询失败:")+curDb->GetLastError();
			return i_ret;
		}
		sRet = SString::toFormat("查询返回:%d行%d列,查询历时:%f秒\r\n", rs.GetRows(), rs.GetColumns(), (soc2 - soc1) + ((float)usec2 - (float)usec1) / 1000000);
	}else
	{
		Rs->sendSiganl((void *)&s_sql,false);
		SDateTime::getSystemTime(soc1, usec1);
		i_ret = curDb->GetMdbClient()->Execute(s_sql);
		SDateTime::getSystemTime(soc2, usec2);
		if (i_ret>=0)
		{
			sRet = SString::toFormat("执行成功,影响%d行,执行历时:%f秒\r\n",i_ret,(soc2 - soc1) + ((float)usec2 - (float)usec1) / 1000000);
		}else
		{
			sRet = "执行失败！ ";
			sRet+=curDb->GetLastError();
			i_ret = -1;
		}
	}
	i_colum = rs.GetColumns();
	i_row = rs.GetRows();
	return i_ret;
}
bool dbInteraction::manageSql(SString s_sql)
{
	s_sql.stripWhiteSpace();
	int soc1, soc2, usec1, usec2,i_ret;

	if (s_sql.left(11).toLower() == "drop table ")
	{
		Rs->sendSiganl((void *)&s_sql,false);
		if (curDb->Execute(s_sql))
		{
			Rs->sendSiganl((void *)&s_sql.Right(s_sql.size()-11),DROP);
			sRet = "表 "+s_sql.Right(s_sql.size()-11)+" 删除成功！";
		}
		else
		{
			sRet = "执行失败！ ";
			sRet+=curDb->GetLastError();
		}
		return true;
		
	}else if(s_sql.left(14).toLower() == "select * from ")
	{
		Rs->sendSiganl((void *)&s_sql,true);
		if (curDb->Retrieve(s_sql,rs)>=0)
		{	
			i_colum = rs.GetColumns();
			i_row = rs.GetRows();
			Rs->sendSiganl((void *)&SString::GetIdAttribute(4,s_sql," "),SELECTALL);
		}else
		{
			sRet = SString("查询失败:")+curDb->GetLastError();
		}
		return true;
	}else if (s_sql.left(12).toLower() == "create table")
	{
		Rs->sendSiganl((void *)&s_sql,false);
		SString s_new = s_sql.right(s_sql.length()-12).stripWhiteSpace();
		SString s_table = s_new.Left(s_new.Find('('));
		if (curDb->Execute(s_sql))
		{
			Rs->sendSiganl((void *)&s_table,CREATE);
		}
		else
		{
			sRet = "执行失败！ ";
			sRet+=curDb->GetLastError();
		}
		return true;
	}
	else
	{
		return false;
	}
}
void dbInteraction::setDbParam(SString sParam)
{
	curDb->SetParams(sParam);
}
void dbInteraction::create(SString sParam)
{
	curDb = new SMdb();
	if (sParam!="")
	{
			setDbParam(sParam);
	}

}
bool dbInteraction::connect()
{
	if (curDb==NULL)
	{
		return false;
	}
	if (curDb->Connect())
	{
		sRet = "连接成功";
		ManConnect::add(curDb);
		return true;
	}else
	{
		sRet = "连接失败";
		if (curDb!=NULL)
		{
			delete curDb;
			curDb = NULL;
		}
		return false;
	}

}
void dbInteraction::add()
{
	//ManConnect::add(curDb);
}
void dbInteraction::addcmd(SString sTable,int row,int colum,SString sValue,SQLTYPE type,SString sRow)
{
	if (curDb==NULL)
	{
		return;
	}
	saveRecord m_sr;
	SString sSql = "";
	bool b=false;
	getTablefields(sTable,TableFields);
	switch(type)
	{
	case 0:
		sSql = "insert into " + sTable;
		sSql+= " values(";
		for(int i = 0;i<colum;i++)
		{
			switch(TableFields[i]->type)
			{
			case 6:
				sSql+="'";
				sSql+=SString::GetIdAttribute(i+1,sValue,",");
				sSql+="'";
				break;
			default:
				sSql+=SString::GetIdAttribute(i+1,sValue,",");
				break;
			}
			if (i<colum-1)
			{
				sSql+=",";
			}
		}
		sSql+=")";
		//execSql(sSql);
		m_sr.Ssql = sSql;
		m_sr.mType =0;
		m_sr.mColum = colum;
		m_sr.mRow = row;
		recordList.push_back(m_sr);
		break;
	case 1:
		sSql = "delete from " + sTable;
		sSql+= " where ";
		for(int i = 0;i<colum;i++)
		{
			sSql+= TableFields[i]->name;
			sSql+= "=";
			switch(TableFields[i]->type)
			{
			case 6:
				sSql+="'";
				sSql+=SString::GetIdAttribute(i+1,sRow,",");
				sSql+="'";
				break;
			default:
				sSql+=SString::GetIdAttribute(i+1,sRow,",");
				break;
			}
			if (i<colum-1)
			{
				sSql+=" and ";
			}
		}
		m_sr.Ssql = sSql;
		m_sr.mType =1;
		m_sr.mColum = colum;
		m_sr.mRow = row;
		recordList.push_back(m_sr);
		break;
	case 2:
		/*SString */sSql = "update "+ sTable + " set ";
		
		for (int i = 0;i<TableFields.count();i++)
		{
			if (i!=colum&&b)
			{
				sSql+= TableFields[i]->name;
				sSql+= "=";
				switch(TableFields[i]->type)
				{
				case 6:
					sSql+="'";
					sSql+=SString::GetIdAttribute(i+1,sRow,",");
					sSql+="'";
					break;
				default:
					sSql+=SString::GetIdAttribute(i+1,sRow,",");
					break;
				}
				if (colum==TableFields.count()-1)
				{
					if (i!=TableFields.count()-2)
					{
						sSql+=" and ";
					}
				}else
				{
					if (i!=TableFields.count()-1)
					{
						sSql+=" and ";
					}
				}

			}else if(i==colum&&!b)
			{
				sSql+= TableFields[i]->name;
				sSql+="=";
				if (SString::GetIdAttribute(colum+1,sRow,",")=="")
				{
					sSql+="'' where ";
				}else
				{
					if (TableFields[i]->type==6)
					{
						sSql+="'";
						sSql+=SString::GetIdAttribute(colum+1,sRow,",")+"' where ";
					}
					else
					{
						sSql+=SString::GetIdAttribute(colum+1,sRow,",")+" where ";
					}
				}
				b=true;
				i=-1;
			}
		}
		m_sr.Ssql = sSql;
		m_sr.mType =2;
		m_sr.mColum = colum;
		m_sr.mRow = row;
		recordList.push_back(m_sr);
		//i_row = curDb->Execute(sSql);
		//if (i_row<0)
		//{
		//	sRet="更新失败！ ";
		//	sRet+=curDb->GetLastError();
		//}else
		//{

		//	sRet="更新成功！";
		//	if (i_row>1)
		//	{
		//		sRet="警告：表中有重复记录!";
		//	}
		//}
		break;
	default:
		m_sr.Ssql = sSql;
		m_sr.mType =type;
		m_sr.mColum = colum;
		m_sr.mRow = row;
		recordList.push_back(m_sr);
		break;
	}
}
bool dbInteraction::isCmdExec()
{
	return recordList.empty();
}
void dbInteraction::runRecord()
{
	for (int i = 0;i<recordList.size();i++)
	{
		if (recordList[i].mType==-1)
		{
			continue;
		}
		execSql(recordList[i].Ssql);
	}
	recordList.clear();
}
void dbInteraction::del(int i_idx)
{
	ManConnect::del(i_idx);
}
void dbInteraction::pointTo(int i_idx)
{
	if (i_idx<0||i_idx>getSize())
	{
		curDb = NULL;
		return;
	}else
	{
		curDb = this->operator[](i_idx);
	}
}
bool dbInteraction::delTable(SString tableName)
{
	return this->execSql("drop table "+tableName);
}
void dbInteraction::registCallback(OnMdbClientTrgCallback pFun, void *cbParam, SString sTableName, BYTE iTriggerFlag)
{
	curDb->GetMdbClient()->RegisterTriggerCallback(pFun,cbParam,sTableName,iTriggerFlag);
}
void dbInteraction::removeCallback(OnMdbClientTrgCallback pFun, void *cbParam, SString sTableName, BYTE iTriggerFlag)
{
	curDb->GetMdbClient()->RemoveTriggerCallback(pFun,cbParam,sTableName,iTriggerFlag);
}
void dbInteraction::RunSqlFile(SString filename)
{
		SFile file(filename);
		sRet.clear();
	if (file.open(IO_ReadOnly) == false)
	{
		sRet="文件";
		sRet+=filename+"不存在！";
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
			if (this->execSql(sql)<0)
			{
				sRet+="SQL["+sql+"]错误:"+curDb->GetLastError()+"\r\n";
				//printf("SQL[%s]错误:%s\n", sql.data(), curDb->GetLastError().data());
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
			if (this->execSql(sql)<0)
			{
				sRet+="SQL["+sql+"]错误:"+curDb->GetLastError()+"\r\n";
				//printf("SQL[%s]错误:%s\n", sql.data(), curDb->GetLastError().data());
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
		sRet+=SString("SQL文件执行完毕!共%d个语句被成功执行!").arg(lines);
		//printf("SQL文件执行完毕!共%d个语句被成功执行!\n", lines);
}
