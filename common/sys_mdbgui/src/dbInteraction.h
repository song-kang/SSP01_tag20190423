//数据库与界面交互类
#ifndef _DBINTERACTION 
#define _DBINTERACTION
#include "SApi.h"
#include "SFile.h"
#include "db/mdb/SMdb.h"
#include "ManConnect.h"
#include "Rsignal.h"
#include <vector>
typedef int SQLTYPE;
typedef struct sendInfo
{
	SString sTable;
	eMdbTriggerType eType;
	int iTrgRows;
	int iRowSize;
	BYTE *pTrgData;
	SString sDate;
	SString sTime;
public:
	~sendInfo()
	{
		if (pTrgData!=NULL)
		{
			delete pTrgData;
			pTrgData =NULL;
		}
	}
}sendInfo;

struct saveRecord
{
	int mRow;
	int mColum;
	SString Ssql;
	SQLTYPE mType;//语句类型 0：insert 1:delete 2:update
};
class dbInteraction:public ManConnect
{
public:
	dbInteraction(void);
	~dbInteraction(void);
	static BYTE * testcall(void* cbParam, SString &sTable, eMdbTriggerType eType,int iTrgRows,int iRowSize,BYTE *pTrgData);
	int execSql(SString s_sql);
	bool manageSql(SString s_sql);//处理特殊语句，用于treewidget是否更新
	template<class Type>Type getField(SString s_table,Type &l)
	{
		if (!curDb->TestConnect())
		{
			sRet = "连接失败";
			return l;
		}
		for (int i = 0;i<i_colum;i++)
		{
			l.push_back(rs.GetColumnName(i).data());
		}
		return l;
	}
	void setDbParam(SString sParam);
	void create(SString sParam="");//创建数据库实例
	bool connect();//连接
	void add();//添加数据库连接对象到链表中
	void addcmd(SString sTable,int row,int colum,SString sValue,SQLTYPE type,SString sRow);//添加执行语句命令
	bool isCmdExec();//判断命令集是否全执行
	void clearRecord()
	{
		recordList.clear();
	}
	void runRecord();//执行命令集中命令
	void del(int i_idx);//从链表中删除
	void pointTo(int i_idx);//指向当前实例
	SString getValue(int row,int colum){return rs.GetValue(row,colum);}
	int getColum(){return i_colum;};
	int getRow(){return i_row;};
	bool delTable(SString tableName);
	void registCallback(OnMdbClientTrgCallback pFun, void *cbParam, SString sTableName, BYTE iTriggerFlag);
	void removeCallback(OnMdbClientTrgCallback pFun, void *cbParam, SString sTableName, BYTE iTriggerFlag);
	SString update(SString sTablename,SString rowData,int colum)
	{
		SString sRet;
		int i_row;
		SPtrList<stuTableField> l_fields;
		curDb->GetMdbClient()->GetTableFields(sTablename.data(),l_fields);
		SString sSql = "update "+ sTablename + " set ";
		bool b=false;
		for (int i = 0;i<l_fields.count();i++)
		{
			if (i!=colum&&b)
			{
				sSql+= l_fields[i]->name;
				sSql+= "=";
				switch(l_fields[i]->type)
				{
				case 6:
					sSql+="'";
					sSql+=SString::GetIdAttribute(i+1,rowData,",");
					sSql+="'";
					break;
				default:
					sSql+=SString::GetIdAttribute(i+1,rowData,",");
					break;
				}
					if (colum==l_fields.count()-1)
					{
						if (i!=l_fields.count()-2)
						{
							sSql+=" and ";
						}
					}else
					{
						if (i!=l_fields.count()-1)
						{
							sSql+=" and ";
						}
					}
				
			}else if(i==colum&&!b)
			{
				sSql+= l_fields[i]->name;
				sSql+="=";
				sSql+=SString::GetIdAttribute(colum+1,rowData,",")+" where ";
				b=true;
				i=-1;
			}
		}
		i_row = curDb->Execute(sSql);
		if (i_row<0)
		{
			sRet="更新失败！ ";
			sRet+=curDb->GetLastError();
		}else
		{

			sRet="更新成功！";
			if (i_row>1)
			{
				sRet="警告：表中有重复记录!";
			}
		}
		return sRet;
	}
	int getTablefields(SString sTable, SPtrList<stuTableField> &TableFields)
	{
		return curDb->GetMdbClient()->GetTableFields(sTable.data(),TableFields);
	}
	CMdbClient * getDbClient()
	{
		return curDb->GetMdbClient();
	}
	void RunSqlFile(SString filename);//运行sql文件
	Rsignal * getRsignal(){return Rs;};
	unsigned int getOID(int row)
	{
		return rs.GetRecord(row)->GetOid();
	}
	SString sRet;//结果显示信息
private:
	SMdb *curDb;//当前数据库指针
	//ManConnect mc;//数据库管理对象
	int i_colum;//列
	int i_row;//行
	SRecordset rs;//数据集
	SPtrList<stuTableField> TableFields;//储存字段信息
	vector<saveRecord> recordList;//执行语句集
	OnMdbClientTrgCallback fp;
	Rsignal *Rs;
};


#endif


